#pragma once

#include "Chicane/Core/FrameTelemetry.hpp"

static constexpr float EVENT_REPEAT_COOLDOWN_IN_MS = 30.0f;

namespace Chicane
{
    namespace Controller
    {
        enum class EventStatus
        {
            Pressed,
            Released
        };

        template<typename E>
        struct Events
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

        template<typename B>
        struct PressableEvents
        {
        public:
            void bind(B inButton, EventStatus inStatus, std::function<void()> inExec)
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

            void exec(B inButton, EventStatus inStatus)
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
                std::clock_t time = std::clock();

                if (FrameTelemetry::deltaToMs(time - m_lastRepeatTime) < EVENT_REPEAT_COOLDOWN_IN_MS)
                {
                    return;
                }

                m_lastRepeatTime  = time;

                for (B button : m_pressed)
                {
                    exec(button, EventStatus::Pressed);
                }
            }

            void clear()
            {
                m_lastRepeatTime = 0;
                m_pressed.clear();
                m_events.clear();
            }

        private:
            void store(B inButton, EventStatus inStatus)
            {
                auto foundButton = std::find(
                    m_pressed.begin(),
                    m_pressed.end(),
                    inButton
                );

                if (inStatus == EventStatus::Pressed)
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
            std::clock_t                                                                               m_lastRepeatTime = 0;
            std::vector<B>                                                                             m_pressed        = {};
            std::unordered_map<B, std::unordered_map<EventStatus, std::vector<std::function<void()>>>> m_events         = {};
        };
    }
}