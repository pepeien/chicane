#include "Chicane/Core/Async.hpp"

namespace Chicane
{
    static std::unordered_map<TimerId, std::shared_ptr<std::atomic<bool>>> m_timers = {};
    static std::mutex                                                      m_mutexTimers;

    static std::atomic<TimerId> m_currentTimerId = { 0 };

    TimerId setTimeout(std::function<void()> inCallback, std::uint32_t inDelayInMs)
    {
        TimerId timerId = m_currentTimerId++;

        auto isActive = std::make_shared<std::atomic<bool>>(true);

        {
            std::lock_guard<std::mutex> lock(m_mutexTimers);
            m_timers[timerId] = isActive;
        }

        std::thread(
            [inCallback, inDelayInMs, timerId, isActive]()
            {
                std::this_thread::sleep_for(std::chrono::milliseconds(inDelayInMs));

                if (isActive->load())
                {
                    inCallback();
                }

                std::lock_guard<std::mutex> lock(m_mutexTimers);

                auto found = m_timers.find(timerId);

                if (found != m_timers.end() && found->second == isActive)
                {
                    m_timers.erase(found);
                }
            }
        ).detach();

        return timerId;
    }

    bool clearTimeout(TimerId inTimerId)
    {
        std::lock_guard<std::mutex> lock(m_mutexTimers);

        auto found = m_timers.find(inTimerId);

        if (found == m_timers.end())
        {
            return false;
        }

        found->second->store(false);

        m_timers.erase(found);

        return true;
    }

    TimerId setInterval(std::function<void()> inCallback, std::uint32_t inIntervalInMs)
    {
        TimerId timerId = m_currentTimerId++;

        auto isActive = std::make_shared<std::atomic<bool>>(true);

        {
            std::lock_guard<std::mutex> lock(m_mutexTimers);

            m_timers[timerId] = isActive;
        }

        std::thread(
            [inCallback, inIntervalInMs, timerId, isActive]()
            {
                while (isActive->load())
                {
                    std::this_thread::sleep_for(std::chrono::milliseconds(inIntervalInMs));

                    if (!isActive->load())
                    {
                        break;
                    }

                    inCallback();
                }

                std::lock_guard<std::mutex> lock(m_mutexTimers);

                auto found = m_timers.find(timerId);

                if (found != m_timers.end() && found->second == isActive)
                {
                    m_timers.erase(found);
                }
            }
        ).detach();

        return timerId;
    }

    bool clearInterval(TimerId inTimerId)
    {
        return clearTimeout(inTimerId);
    }
}