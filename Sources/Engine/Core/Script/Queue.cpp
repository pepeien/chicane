#include "Chicane/Core/Script/Queue.hpp"

namespace Chicane
{
    namespace Script
    {
        void Queue::push(Event inEvent)
        {
            m_pending.push(std::move(inEvent));
        }

        std::vector<Event> Queue::drain()
        {
            return m_pending.drain();
        }
    }
}
