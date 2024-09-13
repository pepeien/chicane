#include "Chicane/Core/Thread/Pool.hpp"

#include "Chicane/Core/Log.hpp"
#include "Chicane/Core/Thread.hpp"

constexpr std::uint32_t MIN_THREAD_COUNT  = 1;
const std::uint32_t MAX_THREAD_COUNT      = std::max(
    std::thread::hardware_concurrency(),
    static_cast<std::uint32_t>(1)
);

namespace Chicane
{
    namespace Thread
    {
        std::uint32_t Pool::getSize()
        {
            return m_threads.size();
        }

        std::uint32_t Pool::getMaxSize()
        {
            return MAX_THREAD_COUNT;
        }

        bool Pool::has(const std::string& inId)
        {
            return m_threads.find(inId) != m_threads.end();
        }

        Instance* Pool::add(const std::string& inId)
        {
            if (m_threads.size() >= MAX_THREAD_COUNT)
            {
                Chicane::Log::warning("There is no space to add the [" + inId + "] thread.");

                return nullptr;
            }

            if (has(inId))
            {
                Chicane::Log::warning("An thread [" + inId + "] already exists.");

                return nullptr;
            }

            wait();

            m_threads.insert(
                std::make_pair(
                    inId,
                    std::make_unique<Instance>()
                )
            );

            return get(inId);
        }

        Instance* Pool::get(const std::string& inId)
        {
            if (!has(inId))
            {
                Chicane::Log::warning("There is no [" + inId + "] thread.");

                return nullptr;
            }

            return m_threads.at(inId).get();
        }

        void Pool::clear()
        {
            wait();

            m_threads.clear();
        }

        void Pool::addJob(const std::string& inId, std::function<void()> inFunction)
        {
            if (!has(inId))
            {
                Chicane::Log::warning("There is no [" + inId + "] thread.");

                return;
            }

            Instance* thread = get(inId);
            thread->addJob(inFunction);
        }

        void Pool::wait()
        {
            for (const auto& [id, thread] : m_threads)
            {
                thread->wait();
            }
        }
    }
}