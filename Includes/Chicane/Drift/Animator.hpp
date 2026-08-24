#pragma once

#include <unordered_map>
#include <vector>

#include "Chicane/Core/String.hpp"

#include "Chicane/Drift.hpp"
#include "Chicane/Drift/Clip.hpp"
#include "Chicane/Drift/Easing/Curve.hpp"
#include "Chicane/Drift/Player.hpp"
#include "Chicane/Drift/Tween.hpp"

namespace Chicane
{
    namespace Drift
    {
        class CHICANE_DRIFT Animator
        {
        public:
            Animator();

        public:
            void tick(float inDeltaTime);

            void tween(
                const String&             inName,
                const std::vector<float>& inFrom,
                const std::vector<float>& inTo,
                float                     inDuration,
                float                     inDelay,
                const EasingCurve&        inEasing
            );

            bool hasTween(const String& inName) const;
            std::vector<float> getTweenValue(const String& inName) const;
            void stopTween(const String& inName);
            void stopTweens();

            void play(const String& inName, const Clip& inClip);
            bool hasPlayer(const String& inName) const;
            Player* getPlayer(const String& inName);
            const Player* getPlayer(const String& inName) const;
            void stopPlayer(const String& inName);
            void stopPlayers();

            void stop();

        private:
            std::unordered_map<String, Tween>  m_tweens;
            std::unordered_map<String, Player> m_players;
        };
    }
}
