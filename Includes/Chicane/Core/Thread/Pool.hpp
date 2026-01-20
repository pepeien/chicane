#pragma once

#include <vector>
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <future>
#include <functional>
#include <atomic>

#include "Chicane/Core.hpp"

namespace Chicane
{
    class CHICANE_CORE ThreadPool
    {
    public:
        explicit ThreadPool(size_t threadCount = std::thread::hardware_concurrency())
            : m_stop(false),
              m_activeTasks(0)
        {
            if (threadCount == 0)
            {
                threadCount = 1;
            }

            for (size_t i = 0; i < threadCount; ++i)
            {
                m_workers.emplace_back([this] {
                    for (;;)
                    {
                        std::function<void()> task;

                        {
                            std::unique_lock<std::mutex> lock(m_mutex);
                            m_cv.wait(lock, [this] { return m_stop || !m_tasks.empty(); });

                            if (m_stop && m_tasks.empty())
                            {
                                return;
                            }

                            task = std::move(m_tasks.front());
                            m_tasks.pop();
                            ++m_activeTasks;
                        }

                        task();

                        {
                            std::lock_guard<std::mutex> lock(m_mutex);
                            --m_activeTasks;
                        }
                        m_doneCv.notify_all();
                    }
                });
            }
        }

        ~ThreadPool() { shutdown(); }

        template <class Func, class... Args>
        auto submit(Func&& func, Args&&... args) -> std::future<std::invoke_result_t<Func, Args...>>
        {
            using ReturnType = std::invoke_result_t<Func, Args...>;

            auto task        = std::make_shared<std::packaged_task<ReturnType()>>(
                std::bind(std::forward<Func>(func), std::forward<Args>(args)...)
            );

            std::future<ReturnType> result = task->get_future();

            {
                std::lock_guard<std::mutex> lock(m_mutex);
                if (m_stop)
                {
                    throw std::runtime_error("submit on stopped ThreadPool");
                }

                m_tasks.emplace([task]() { (*task)(); });
            }

            m_cv.notify_one();
            return result;
        }

        void wait()
        {
            std::unique_lock<std::mutex> lock(m_mutex);
            m_doneCv.wait(lock, [this] { return m_tasks.empty() && m_activeTasks == 0; });
        }

        void shutdown()
        {
            {
                std::lock_guard<std::mutex> lock(m_mutex);
                if (m_stop)
                {
                    return;
                }
                m_stop = true;
            }

            m_cv.notify_all();

            for (auto& worker : m_workers)
            {
                if (worker.joinable())
                {
                    worker.join();
                }
            }
        }

    private:
        std::vector<std::thread>          m_workers;
        std::queue<std::function<void()>> m_tasks;

        std::mutex                        m_mutex;
        std::condition_variable           m_cv;
        std::condition_variable           m_doneCv;

        bool                              m_stop;
        size_t                            m_activeTasks;
    };
}