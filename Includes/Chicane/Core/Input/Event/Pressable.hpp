#pragma once

#include <algorithm>
#include <functional>
#include <vector>
#include <unordered_map>

#include "Chicane/Core.hpp"
#include "Chicane/Core/Input/Event/Events.hpp"
#include "Chicane/Core/Input/Status.hpp"
#include "Chicane/Core/Recorder.hpp"

static constexpr inline const float COOLDOWN_IN_MS = 30.0f;

namespace Chicane
{
    namespace Input
    {
        template <typename B>
        struct CHICANE_CORE PressableEvents : private Recorder
        {
        public:
            using Events = std::unordered_map<B, std::unordered_map<Status, std::vector<std::function<void()>>>>;

        public:
            inline PressableEvents()
                : Recorder(COOLDOWN_IN_MS),
                  m_pressed({}),
                  m_events({})
            {}

        protected:
            inline void onTime() override
            {
                for (B button : m_pressed)
                {
                    exec(button, Status::Pressed);
                }
            }

        public:
            inline void bind(B inButton, Status inStatus, std::function<void()> inExec)
            {
                if (m_events.find(inButton) == m_events.end())
                {
                    m_events[inButton] = {};
                }

                auto& events = m_events.at(inButton);

                if (events.find(inStatus) == events.end())
                {
                    events[inStatus] = {};
                }

                events.at(inStatus).push_back(inExec);
            }

            inline void exec(B inButton, Status inStatus)
            {
                store(inButton, inStatus);

                if (m_events.find(inButton) == m_events.end())
                {
                    return;
                }

                auto& events = m_events.at(inButton);

                if (events.find(inStatus) == events.end())
                {
                    return;
                }

                for (auto& function : events.at(inStatus))
                {
                    function();
                }
            }

            inline void repeat() { end(); }

            inline void clear()
            {
                m_pressed.clear();
                m_events.clear();
            }

        private:
            inline void store(B inButton, Status inStatus)
            {
                auto foundButton = std::find(m_pressed.begin(), m_pressed.end(), inButton);

                if (inStatus == Status::Pressed)
                {
                    if (foundButton == m_pressed.end())
                    {
                        m_pressed.push_back(inButton);
                    }

                    return;
                }

                if (foundButton == m_pressed.end())
                {
                    return;
                }

                m_pressed.erase(foundButton);
            }

        private:
            std::vector<B> m_pressed;
            Events         m_events;
        };
    }
}