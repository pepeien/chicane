#include "Chicane/Core/Worker/Pool/Parallel.hpp"

#include <algorithm>

namespace Chicane
{
    static thread_local bool g_bWorkerPoolParallel = false;

    bool WorkerPoolParallel::sIsWorker()
    {
        return g_bWorkerPoolParallel;
    }

    WorkerPoolParallel::~WorkerPoolParallel()
    {
        shutdown();
    }

    void WorkerPoolParallel::run(std::size_t inCount, const Job& inJob)
    {
        if (inCount == 0 || !inJob)
        {
            return;
        }

        unsigned cores = std::thread::hardware_concurrency();
        if (cores < 1)
        {
            cores = 1;
        }

        const bool bFew = inCount <= static_cast<std::size_t>(cores);
        if (inCount == 1 || bFew || g_bWorkerPoolParallel)
        {
            for (std::size_t index = 0; index < inCount; index++)
            {
                inJob(index);
            }

            return;
        }

        ensureWorkers();

        std::shared_ptr<Batch> batch = std::make_shared<Batch>();
        batch->job                   = inJob;
        batch->count                 = inCount;
        batch->remaining             = inCount;

        bool bSerial = false;
        {
            std::lock_guard<std::mutex> lock(m_mutex);
            const bool                  bWorkersEmptyOrShutdown = static_cast<bool>(m_workers.empty() || m_bShutdown);

            if (bWorkersEmptyOrShutdown)
            {
                bSerial = true;
            }

            if (!bWorkersEmptyOrShutdown)
            {
                m_batches.push_back(batch);
            }
        }

        if (bSerial)
        {
            for (std::size_t index = 0; index < inCount; index++)
            {
                inJob(index);
            }

            return;
        }

        m_signal.notify_all();

        std::exception_ptr error;
        {
            std::unique_lock<std::mutex> lock(m_mutex);
            batch->done.wait(lock, [&batch]() { return batch->remaining == 0; });
            error = batch->error;
        }

        if (error)
        {
            std::rethrow_exception(error);
        }
    }

    void WorkerPoolParallel::detach(std::function<void()> inJob)
    {
        if (!inJob)
        {
            return;
        }

        const bool bInline = g_bWorkerPoolParallel;
        if (bInline)
        {
            inJob();

            return;
        }

        ensureWorkers();

        std::shared_ptr<Batch> batch = std::make_shared<Batch>();
        batch->job                   = [inJob](std::size_t) { inJob(); };
        batch->count                 = 1;
        batch->remaining             = 1;

        bool bRunHere = false;
        {
            std::lock_guard<std::mutex> lock(m_mutex);
            const bool                  bWorkersEmptyOrShutdown = static_cast<bool>(m_workers.empty() || m_bShutdown);

            if (bWorkersEmptyOrShutdown)
            {
                bRunHere = true;
            }

            if (!bWorkersEmptyOrShutdown)
            {
                m_batches.push_back(batch);
            }
        }

        if (bRunHere)
        {
            inJob();

            return;
        }

        m_signal.notify_all();
    }

    void WorkerPoolParallel::shutdown()
    {
        {
            std::lock_guard<std::mutex> lock(m_mutex);
            m_bShutdown = true;
        }

        m_signal.notify_all();

        for (std::thread& worker : m_workers)
        {
            if (worker.joinable())
            {
                worker.join();
            }
        }
    }

    void WorkerPoolParallel::ensureWorkers()
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        if (!m_workers.empty() || m_bShutdown)
        {
            return;
        }

        unsigned count = std::thread::hardware_concurrency();
        if (count < 1)
        {
            count = 1;
        }

        m_workers.reserve(count);
        for (unsigned index = 0; index < count; index++)
        {
            m_workers.emplace_back(&WorkerPoolParallel::loop, this);
        }
    }

    bool WorkerPoolParallel::hasWork() const
    {
        for (const std::shared_ptr<Batch>& batch : m_batches)
        {
            if (batch && batch->next < batch->count)
            {
                return true;
            }
        }

        return false;
    }

    void WorkerPoolParallel::loop()
    {
        g_bWorkerPoolParallel = true;

        while (true)
        {
            std::shared_ptr<Batch> batch;
            std::size_t            index = 0;
            Job                    job;

            {
                std::unique_lock<std::mutex> lock(m_mutex);
                m_signal.wait(lock, [this]() { return m_bShutdown || hasWork(); });

                if (!hasWork())
                {
                    if (m_bShutdown)
                    {
                        return;
                    }

                    continue;
                }

                for (const std::shared_ptr<Batch>& candidate : m_batches)
                {
                    if (!candidate || candidate->next >= candidate->count)
                    {
                        continue;
                    }

                    batch = candidate;
                    index = batch->next;
                    batch->next++;
                    job = batch->job;

                    break;
                }
            }

            if (!batch)
            {
                continue;
            }

            if (job)
            {
                try
                {
                    job(index);
                }
                catch (...)
                {
                    std::lock_guard<std::mutex> lock(m_mutex);
                    if (!batch->error)
                    {
                        batch->error = std::current_exception();
                    }
                }
            }

            {
                std::lock_guard<std::mutex> lock(m_mutex);
                if (batch->remaining > 0)
                {
                    batch->remaining--;
                }

                if (batch->remaining == 0)
                {
                    const auto found = std::find(m_batches.begin(), m_batches.end(), batch);
                    if (found != m_batches.end())
                    {
                        m_batches.erase(found);
                    }

                    batch->done.notify_one();
                    m_signal.notify_all();
                }
            }
        }
    }
}
