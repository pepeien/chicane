#pragma once

#include <condition_variable>
#include <cstddef>
#include <exception>
#include <functional>
#include <mutex>
#include <queue>
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
        static bool isWorker();

    public:
        WorkerPoolParallel() = default;
        ~WorkerPoolParallel();

    public:
        void run(std::size_t inCount, const Job& inJob);

    private:
        void shutdown();
        void ensureWorkers();
        void loop();

    private:
        std::mutex               m_mutex;
        std::condition_variable  m_signal;
        std::condition_variable  m_done;
        std::queue<std::size_t>  m_indices;
        Job                      m_job;
        std::exception_ptr       m_error;
        std::vector<std::thread> m_workers;
        std::size_t              m_remaining = 0;
        bool                     m_bActive   = false;
        bool                     m_bShutdown = false;
    };
}
