#include "Chicane/Core/Worker.hpp"

namespace Chicane
{
    Worker& Worker::instance()
    {
        static Worker worker;

        return worker;
    }

    void Worker::submit(Job inJob)
    {
        instance().enqueue(std::move(inJob));
    }

    Worker::Worker()
        : m_bIsRunning(false)
    {}

    Worker::~Worker()
    {
        {
            std::lock_guard<std::mutex> lock(m_mutex);
            m_bIsRunning = false;
        }

        m_signal.notify_all();

        if (m_thread.joinable())
        {
            m_thread.join();
        }
    }

    void Worker::enqueue(Job inJob)
    {
        if (!inJob)
        {
            return;
        }

        bool bShouldStart = false;
        {
            std::lock_guard<std::mutex> lock(m_mutex);
            m_pending.push(std::move(inJob));
            if (!m_bIsRunning)
            {
                m_bIsRunning = true;
                bShouldStart = true;
            }
        }

        if (bShouldStart)
        {
            m_thread = std::thread(&Worker::loop, this);
        }

        m_signal.notify_one();
    }

    void Worker::loop()
    {
        while (true)
        {
            Job job;

            {
                std::unique_lock<std::mutex> lock(m_mutex);
                m_signal.wait(lock, [this]() { return !m_bIsRunning || !m_pending.empty(); });

                if (!m_bIsRunning && m_pending.empty())
                {
                    return;
                }

                job = std::move(m_pending.front());
                m_pending.pop();
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
