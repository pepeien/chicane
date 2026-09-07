#pragma once

#include <condition_variable>
#include <functional>
#include <mutex>
#include <queue>
#include <thread>

#include "Chicane/Core.hpp"

namespace Chicane
{
    class CHICANE_CORE Worker
    {
    public:
        using Job = std::function<void()>;

    public:
        static Worker& instance();
        static void submit(Job inJob);

    public:
        Worker();
        ~Worker();

    public:
        void enqueue(Job inJob);

    private:
        void loop();

    private:
        std::mutex              m_mutex;
        std::condition_variable m_signal;
        std::queue<Job>         m_pending;
        std::thread             m_thread;
        bool                    m_bIsRunning;
    };
}
