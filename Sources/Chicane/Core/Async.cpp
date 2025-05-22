#include "Chicane/Core/Async.hpp"

namespace Chicane
{
    static std::unordered_map<TimerId, std::shared_ptr<std::atomic<bool>>> g_timers = {};
    static std::mutex                                                      g_mutexTimers;

    static std::atomic<TimerId> g_currentTimerId = { 0 };

    TimerId setTimeout(std::function<void()> inCallback, std::uint32_t inDelayInMs)
    {
        TimerId timerId = g_currentTimerId++;

        auto isActive = std::make_shared<std::atomic<bool>>(true);

        {
            std::lock_guard<std::mutex> lock(g_mutexTimers);
            g_timers[timerId] = isActive;
        }

        std::thread(
            [inCallback, inDelayInMs, timerId, isActive]()
            {
                std::this_thread::sleep_for(std::chrono::milliseconds(inDelayInMs));

                if (isActive->load())
                {
                    inCallback();
                }

                std::lock_guard<std::mutex> lock(g_mutexTimers);

                auto found = g_timers.find(timerId);

                if (found != g_timers.end() && found->second == isActive)
                {
                    g_timers.erase(found);
                }
            }
        ).detach();

        return timerId;
    }

    bool clearTimeout(TimerId inTimerId)
    {
        std::lock_guard<std::mutex> lock(g_mutexTimers);

        auto found = g_timers.find(inTimerId);

        if (found == g_timers.end())
        {
            return false;
        }

        found->second->store(false);

        g_timers.erase(found);

        return true;
    }

    TimerId setInterval(std::function<void()> inCallback, std::uint32_t inIntervalInMs)
    {
        TimerId timerId = g_currentTimerId++;

        auto isActive = std::make_shared<std::atomic<bool>>(true);

        {
            std::lock_guard<std::mutex> lock(g_mutexTimers);

            g_timers[timerId] = isActive;
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

                std::lock_guard<std::mutex> lock(g_mutexTimers);

                auto found = g_timers.find(timerId);

                if (found != g_timers.end() && found->second == isActive)
                {
                    g_timers.erase(found);
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