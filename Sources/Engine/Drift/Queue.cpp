#include "Chicane/Drift/Queue.hpp"

#include <algorithm>

namespace Chicane
{
    namespace Drift
    {
        Queue::Queue()
            : m_clips({}),
              m_pending({}),
              m_playing(""),
              m_player()
        {}

        void Queue::add(const Clip& inClip)
        {
            if (inClip.name.isEmpty())
            {
                return;
            }

            m_clips.insert_or_assign(inClip.name, inClip);

            if (m_playing.equals(inClip.name))
            {
                start(inClip.name);
            }
        }

        void Queue::remove(const String& inName)
        {
            m_clips.erase(inName);
            m_pending.erase(std::remove(m_pending.begin(), m_pending.end(), inName), m_pending.end());

            if (!m_playing.equals(inName))
            {
                return;
            }

            if (m_pending.empty())
            {
                stop();

                return;
            }

            advance();
        }

        void Queue::clear()
        {
            m_clips.clear();
            stop();
        }

        bool Queue::has(const String& inName) const
        {
            return m_clips.find(inName) != m_clips.end();
        }

        const Clip* Queue::get(const String& inName) const
        {
            const auto found = m_clips.find(inName);
            if (found == m_clips.end())
            {
                return nullptr;
            }

            return &found->second;
        }

        void Queue::play(const String& inName)
        {
            m_pending.clear();
            start(inName);
        }

        void Queue::enqueue(const String& inName)
        {
            if (!has(inName))
            {
                return;
            }

            if (m_playing.isEmpty() || m_player.isFinished() || m_player.isStopped())
            {
                start(inName);

                return;
            }

            m_pending.push_back(inName);
        }

        void Queue::stop()
        {
            m_pending.clear();
            m_playing = "";
            m_player.stop();
        }

        void Queue::tick(float inDeltaTime)
        {
            if (m_playing.isEmpty())
            {
                return;
            }

            m_player.tick(inDeltaTime);

            if (m_player.isFinished() && !m_pending.empty())
            {
                advance();
            }
        }

        bool Queue::isPlaying() const
        {
            return !m_playing.isEmpty();
        }

        bool Queue::isIdle() const
        {
            return m_playing.isEmpty() && m_pending.empty();
        }

        const String& Queue::getPlaying() const
        {
            return m_playing;
        }

        Player* Queue::getPlayer()
        {
            if (m_playing.isEmpty())
            {
                return nullptr;
            }

            return &m_player;
        }

        const Player* Queue::getPlayer() const
        {
            if (m_playing.isEmpty())
            {
                return nullptr;
            }

            return &m_player;
        }

        std::vector<float> Queue::sample(const String& inTrack) const
        {
            if (m_playing.isEmpty())
            {
                return {};
            }

            return m_player.sample(inTrack);
        }

        void Queue::start(const String& inName)
        {
            const auto found = m_clips.find(inName);
            if (found == m_clips.end())
            {
                return;
            }

            m_player = Player();
            m_player.setClip(found->second);
            m_player.setFillForwards(true);
            m_player.reset();
            m_player.play();
            m_playing = inName;
        }

        void Queue::advance()
        {
            while (!m_pending.empty())
            {
                const String next = m_pending.front();
                m_pending.erase(m_pending.begin());

                if (!has(next))
                {
                    continue;
                }

                start(next);

                return;
            }

            stop();
        }
    }
}
