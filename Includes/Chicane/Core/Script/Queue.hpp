#pragma once

#include <vector>

#include "Chicane/Core.hpp"
#include "Chicane/Core/Mailbox.hpp"
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
            Mailbox<Event> m_pending;
        };
    }
}
