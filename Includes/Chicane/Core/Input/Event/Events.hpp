#pragma once

#include "Chicane/Core/Essential.hpp"

namespace Chicane
{
    namespace Input
    {
        template<typename E>
        struct CHICANE_CORE Events
        {
        public:
            void bind(std::function<void(const E&)> inExec)
            {
                m_execs.push_back(inExec);
            }

            void exec(const E& inEvent)
            {
                for (auto& function : m_execs)
                {
                    function(inEvent);
                }
            }

            void clear()
            {
                m_execs.clear();
            }

        private:
            std::vector<std::function<void(const E&)>> m_execs = {};
        };
    }
}