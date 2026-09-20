#pragma once

#include <array>
#include <bitset>
#include <cstddef>
#include <vector>

#include "Chicane/Drift/Clip.hpp"
#include "Chicane/Drift/Easing/Curve.hpp"
#include "Chicane/Drift/Player.hpp"

#include "Chicane/Grid.hpp"
#include "Chicane/Grid/Style/Animator/Track.hpp"
#include "Chicane/Grid/Style/Animator/Tween.hpp"
#include "Chicane/Grid/Style/Property/Dirty.hpp"
#include "Chicane/Grid/Style/Property/Id.hpp"
#include "Chicane/Grid/Style/Property/Table.hpp"

namespace Chicane
{
    namespace Grid
    {
        class CHICANE_GRID StyleAnimator
        {
        public:
            using Mask  = std::bitset<StylePropertyTable::COUNT>;
            using Slots = std::array<std::size_t, StylePropertyTable::COUNT>;

        public:
            static constexpr inline const std::size_t INVALID_SLOT = static_cast<std::size_t>(-1);

        public:
            StyleAnimator();

        public:
            void tick(float inDeltaTime);

            void tween(
                StylePropertyId           inId,
                const float*              inFrom,
                const float*              inTo,
                float                     inDuration,
                float                     inDelay,
                const Drift::EasingCurve& inEasing
            );

            bool hasTween(StylePropertyId inId) const;
            bool readTween(StylePropertyId inId, float* outValues) const;
            void stopTween(StylePropertyId inId);
            void stopTweens();

            const std::vector<StyleAnimatorTween>& getTweens() const;

            void play(const Drift::Clip& inClip);
            bool hasPlayer() const;
            Drift::Player* getPlayer();
            const Drift::Player* getPlayer() const;
            void stopPlayer();

            const std::vector<StyleAnimatorTrack>& getPlayerTracks() const;

            void stop();
            bool isIdle() const;

            StylePropertyDirty getDirty() const;

        private:
            void refreshDirty();

        private:
            Mask                            m_active;
            Slots                           m_slots;
            std::vector<StyleAnimatorTween> m_tweens;

            Drift::Player                   m_player;
            std::vector<StyleAnimatorTrack> m_playerTracks;
            bool                            m_bHasPlayer;

            StylePropertyDirty              m_dirty;
        };
    }
}
