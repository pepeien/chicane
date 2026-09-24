#include "Chicane/Core/Worker/Pool/Parallel.hpp"

namespace Chicane
{
    static thread_local bool g_bWorkerPoolParallel = false;

    bool WorkerPoolParallel::isWorker()
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

        if (inCount == 1 || g_bWorkerPoolParallel)
        {
            for (std::size_t index = 0; index < inCount; index++)
            {
                inJob(index);
            }

            return;
        }

        ensureWorkers();

        bool bSerial = false;
        {
            std::lock_guard<std::mutex> lock(m_mutex);
            if (m_workers.empty() || m_bShutdown)
            {
                bSerial = true;
            }
            else
            {
                m_job       = inJob;
                m_error     = nullptr;
                m_remaining = inCount;
                m_bActive   = true;

                for (std::size_t index = 0; index < inCount; index++)
                {
                    m_indices.push(index);
                }
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
            m_done.wait(lock, [this]() { return m_remaining == 0; });
            m_bActive = false;
            m_job     = nullptr;
            error     = m_error;
            m_error   = nullptr;
        }

        if (error)
        {
            std::rethrow_exception(error);
        }
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

    void WorkerPoolParallel::loop()
    {
        g_bWorkerPoolParallel = true;

        while (true)
        {
            std::size_t index = 0;
            Job         job;

            {
                std::unique_lock<std::mutex> lock(m_mutex);
                m_signal.wait(lock, [this]() { return m_bShutdown || (m_bActive && !m_indices.empty()); });

                if (m_indices.empty())
                {
                    if (m_bShutdown)
                    {
                        return;
                    }

                    continue;
                }

                index = m_indices.front();
                m_indices.pop();
                job = m_job;
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
                    if (!m_error)
                    {
                        m_error = std::current_exception();
                    }
                }
            }

            {
                std::lock_guard<std::mutex> lock(m_mutex);
                if (m_remaining > 0)
                {
                    m_remaining--;
                }

                if (m_remaining == 0)
                {
                    m_bActive = false;
                    m_done.notify_one();
                }
            }
        }
    }
}
