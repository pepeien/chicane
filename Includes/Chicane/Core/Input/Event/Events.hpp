#pragma once

#include "Core/Essentials.hpp"

namespace Chicane
{
    namespace Input
    {
        template<typename E>
        struct CHICANE Events
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