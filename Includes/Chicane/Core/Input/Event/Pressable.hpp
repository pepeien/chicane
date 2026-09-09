#pragma once

#include <algorithm>
#include <functional>
#include <vector>
#include <unordered_map>

#include "Chicane/Core.hpp"
#include "Chicane/Core/Input/Event/Events.hpp"
#include "Chicane/Core/Input/Event/Pressable/Pressed.hpp"
#include "Chicane/Core/Input/Status.hpp"
#include "Chicane/Core/Time.hpp"

namespace Chicane
{
    namespace Input
    {
        template <typename B>
        struct PressableEvents
        {
        public:
            using Events = std::unordered_map<B, std::unordered_map<Status, std::vector<std::function<void()>>>>;

        public:
            static constexpr float REPEAT_INTERVAL_MS = 16.0f;

        public:
            inline PressableEvents()
                : m_pressed({}),
                  m_events({})
            {}

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

                if (inStatus != Status::Pressed)
                {
                    fire(inButton, inStatus);

                    return;
                }

                firePressed();
            }

            inline void repeat()
            {
                if (m_pressed.empty())
                {
                    return;
                }

                const Time::Point now  = Time::Clock::now();
                bool              bDue = false;
                for (const PressablePressed<B>& entry : m_pressed)
                {
                    if (Time::miliseconds(now - entry.lastFire) >= REPEAT_INTERVAL_MS)
                    {
                        bDue = true;

                        break;
                    }
                }

                if (!bDue)
                {
                    return;
                }

                firePressed();
            }

            inline void clear()
            {
                m_pressed.clear();
                m_events.clear();
            }

        private:
            inline void fire(B inButton, Status inStatus)
            {
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

            inline void firePressed()
            {
                const Time::Point now = Time::Clock::now();
                std::vector<B>    toFire;
                toFire.reserve(m_pressed.size());

                for (PressablePressed<B>& entry : m_pressed)
                {
                    entry.lastFire = now;
                    toFire.push_back(entry.button);
                }

                for (B button : toFire)
                {
                    fire(button, Status::Pressed);
                }
            }

            inline typename std::vector<PressablePressed<B>>::iterator findPressed(B inButton)
            {
                return std::find_if(
                    m_pressed.begin(),
                    m_pressed.end(),
                    [inButton](const PressablePressed<B>& inEntry) { return inEntry.button == inButton; }
                );
            }

            inline void store(B inButton, Status inStatus)
            {
                auto foundButton = findPressed(inButton);

                if (inStatus == Status::Pressed)
                {
                    if (foundButton == m_pressed.end())
                    {
                        m_pressed.push_back({inButton, Time::Clock::now()});
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
            std::vector<PressablePressed<B>> m_pressed;
            Events                           m_events;
        };
    }
}
