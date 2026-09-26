#include "Chicane/Core/Script/Queue.hpp"

namespace Chicane
{
    namespace Script
    {
        void Queue::push(Event inEvent)
        {
            std::lock_guard<std::mutex> lock(m_mutex);
            m_pending.push_back(std::move(inEvent));
        }

        std::vector<Event> Queue::drain()
        {
            std::lock_guard<std::mutex> lock(m_mutex);
            std::vector<Event>          pending = std::move(m_pending);
            m_pending.clear();

            return pending;
        }
    }
}
