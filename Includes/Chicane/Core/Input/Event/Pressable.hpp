#pragma once

#include "Chicane/Core/Essentials.hpp"
#include "Chicane/Core/FrameTelemetry.hpp"
#include "Chicane/Core/Input/Event/Events.hpp"
#include "Chicane/Core/Input/Event/Status.hpp"

static constexpr float EVENT_REPEAT_COOLDOWN_IN_MS = 30.0f;

namespace Chicane
{
    namespace Input
    {
        template<typename B>
        struct CHICANE PressableEvents
        {
        public:
            typedef std::unordered_map<B, std::unordered_map<Event::Status, std::vector<std::function<void()>>>> Events;

        public:
            void bind(B inButton, Event::Status inStatus, std::function<void()> inExec)
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

            void exec(B inButton, Event::Status inStatus)
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

            void repeat()
            {
                m_timer.endCapture();

                if (m_timer.delta < EVENT_REPEAT_COOLDOWN_IN_MS)
                {
                    return;
                }

                m_timer.startCapture();

                for (B button : m_pressed)
                {
                    exec(button, Event::Status::Pressed);
                }
            }

            void clear()
            {
                m_pressed.clear();
                m_events.clear();
            }

        private:
            void store(B inButton, Event::Status inStatus)
            {
                auto foundButton = std::find(
                    m_pressed.begin(),
                    m_pressed.end(),
                    inButton
                );

                if (inStatus == Event::Status::Pressed)
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
            FrameTelemetry m_timer   = {};
            std::vector<B> m_pressed = {};
            Events         m_events  = {};
        };
    }
}