#pragma once

#include <mutex>
#include <vector>

#include "Chicane/Core.hpp"
#include "Chicane/Core/Script/Event.hpp"

namespace Chicane
{
    namespace Script
    {
        class CHICANE_CORE Queue
        {
        public:
            void push(Event inEvent);
            std::vector<Event> drain();

        private:
            std::mutex         m_mutex;
            std::vector<Event> m_pending;
        };
    }
}
