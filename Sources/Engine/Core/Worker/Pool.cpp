#include "Chicane/Core/Worker/Pool.hpp"

namespace Chicane
{
    void WorkerPool::submit(Job inJob)
    {
        getInstance().enqueue(std::move(inJob));
    }

    void WorkerPool::parallel(std::size_t inCount, const WorkerPoolParallel::Job& inJob)
    {
        getInstance().m_parallel.run(inCount, inJob);
    }

    WorkerPool::WorkerPool() = default;

    WorkerPool::~WorkerPool()
    {
        {
            std::lock_guard<std::mutex> lock(m_mutex);
            m_bShutdown = true;
        }

        m_signal.notify_all();

        if (m_coordinator.joinable())
        {
            m_coordinator.join();
        }
    }

    void WorkerPool::enqueue(Job inJob)
    {
        if (!inJob)
        {
            return;
        }

        bool bShouldStart = false;
        {
            std::lock_guard<std::mutex> lock(m_mutex);
            if (m_bShutdown)
            {
                return;
            }

            m_jobs.push(std::move(inJob));
            if (!m_bCoordinator)
            {
                m_bCoordinator = true;
                bShouldStart   = true;
            }
        }

        if (bShouldStart)
        {
            m_coordinator = std::thread(&WorkerPool::coordinator, this);
        }

        m_signal.notify_one();
    }

    void WorkerPool::coordinator()
    {
        while (true)
        {
            Job job;

            {
                std::unique_lock<std::mutex> lock(m_mutex);
                m_signal.wait(lock, [this]() { return m_bShutdown || !m_jobs.empty(); });

                if (m_bShutdown && m_jobs.empty())
                {
                    return;
                }

                job = std::move(m_jobs.front());
                m_jobs.pop();
            }

            if (!job)
            {
                continue;
            }

            try
            {
                job();
            }
            catch (...)
            {}
        }
    }
}
