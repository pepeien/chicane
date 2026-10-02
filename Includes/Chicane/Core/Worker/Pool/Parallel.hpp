#pragma once

#include <condition_variable>
#include <cstddef>
#include <deque>
#include <exception>
#include <functional>
#include <memory>
#include <mutex>
#include <thread>
#include <vector>

#include "Chicane/Core.hpp"

namespace Chicane
{
    class CHICANE_CORE WorkerPoolParallel
    {
    public:
        using Job = std::function<void(std::size_t)>;

    public:
        static bool sIsWorker();

    public:
        WorkerPoolParallel() = default;
        ~WorkerPoolParallel();

    public:
        void run(std::size_t inCount, const Job& inJob);
        void detach(std::function<void()> inJob);

    private:
        struct Batch
        {
            Job                     job;
            std::size_t             next      = 0;
            std::size_t             count     = 0;
            std::size_t             remaining = 0;
            std::exception_ptr      error;
            std::condition_variable done;
        };

    private:
        void shutdown();
        void ensureWorkers();
        void loop();
        bool hasWork() const;

    private:
        std::mutex                         m_mutex;
        std::condition_variable            m_signal;
        std::deque<std::shared_ptr<Batch>> m_batches;
        std::vector<std::thread>           m_workers;
        bool                               m_bShutdown = false;
    };
}
