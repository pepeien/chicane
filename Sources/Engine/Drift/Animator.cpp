#include "Chicane/Drift/Animator.hpp"

#include <cmath>

namespace Chicane
{
    namespace Drift
    {
        static bool areNear(const std::vector<float>& inLeft, const std::vector<float>& inRight)
        {
            if (inLeft.size() != inRight.size())
            {
                return false;
            }

            for (std::size_t i = 0; i < inLeft.size(); i++)
            {
                if (std::fabs(inLeft.at(i) - inRight.at(i)) > 0.001f)
                {
                    return false;
                }
            }

            return true;
        }

        Animator::Animator()
            : m_tweens({}),
              m_players({})
        {}

        void Animator::tick(float inDeltaTime)
        {
            for (auto& [name, tween] : m_tweens)
            {
                tween.tick(inDeltaTime);
            }

            for (auto iterator = m_tweens.begin(); iterator != m_tweens.end();)
            {
                if (iterator->second.isFinished())
                {
                    iterator = m_tweens.erase(iterator);

                    continue;
                }

                iterator++;
            }

            for (auto& [name, player] : m_players)
            {
                player.tick(inDeltaTime);
            }

            for (auto iterator = m_players.begin(); iterator != m_players.end();)
            {
                if (iterator->second.isFinished() && !iterator->second.fillsForwards())
                {
                    iterator = m_players.erase(iterator);

                    continue;
                }

                iterator++;
            }
        }

        void Animator::tween(
            const String&             inName,
            const std::vector<float>& inFrom,
            const std::vector<float>& inTo,
            float                     inDuration,
            float                     inDelay,
            const EasingCurve&        inEasing
        )
        {
            auto found = m_tweens.find(inName);

            if (found != m_tweens.end())
            {
                if (areNear(found->second.getTo(), inTo) && found->second.isPlaying())
                {
                    return;
                }

                found->second.setFrom(found->second.hasStarted() ? found->second.getValue() : inFrom);
                found->second.setTo(inTo);
                found->second.setDuration(inDuration);
                found->second.setDelay(inDelay);
                found->second.setEasing(inEasing);
                found->second.reset();
                found->second.play();

                return;
            }

            Tween tween;
            tween.setFrom(inFrom);
            tween.setTo(inTo);
            tween.setDuration(inDuration);
            tween.setDelay(inDelay);
            tween.setEasing(inEasing);
            tween.reset();
            tween.play();

            m_tweens.insert_or_assign(inName, tween);
        }

        bool Animator::hasTween(const String& inName) const
        {
            return m_tweens.find(inName) != m_tweens.end();
        }

        std::vector<float> Animator::getTweenValue(const String& inName) const
        {
            const auto found = m_tweens.find(inName);

            if (found == m_tweens.end())
            {
                return {};
            }

            return found->second.getValue();
        }

        void Animator::stopTween(const String& inName)
        {
            m_tweens.erase(inName);
        }

        void Animator::stopTweens()
        {
            m_tweens.clear();
        }

        void Animator::play(const String& inName, const Clip& inClip)
        {
            Player player;
            player.setClip(inClip);
            player.reset();
            player.play();

            m_players.insert_or_assign(inName, player);
        }

        bool Animator::hasPlayer(const String& inName) const
        {
            return m_players.find(inName) != m_players.end();
        }

        Player* Animator::getPlayer(const String& inName)
        {
            const auto found = m_players.find(inName);

            if (found == m_players.end())
            {
                return nullptr;
            }

            return &found->second;
        }

        const Player* Animator::getPlayer(const String& inName) const
        {
            const auto found = m_players.find(inName);

            if (found == m_players.end())
            {
                return nullptr;
            }

            return &found->second;
        }

        void Animator::stopPlayer(const String& inName)
        {
            m_players.erase(inName);
        }

        void Animator::stopPlayers()
        {
            m_players.clear();
        }

        void Animator::stop()
        {
            stopTweens();
            stopPlayers();
        }
    }
}
