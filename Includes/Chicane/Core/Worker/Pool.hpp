#pragma once

#include <condition_variable>
#include <functional>
#include <mutex>
#include <queue>
#include <thread>

#include "Chicane/Core.hpp"
#include "Chicane/Core/Worker/Pool/Parallel.hpp"

namespace Chicane
{
    class CHICANE_CORE WorkerPool
    {
    public:
        using Job = std::function<void()>;

    public:
        static inline WorkerPool& getInstance()
        {
            static WorkerPool result;

            return result;
        }

        static void submit(Job inJob);
        static void parallel(std::size_t inCount, const WorkerPoolParallel::Job& inJob);

    public:
        WorkerPool();
        ~WorkerPool();

    private:
        void enqueue(Job inJob);
        void coordinator();

    private:
        std::mutex              m_mutex;
        std::condition_variable m_signal;
        std::queue<Job>         m_jobs;
        std::thread             m_coordinator;
        bool                    m_bCoordinator = false;
        bool                    m_bShutdown    = false;

        WorkerPoolParallel      m_parallel;
    };
}
