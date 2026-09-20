#include "Chicane/Grid/Style/Animator.hpp"

#include <cmath>

#include "Chicane/Drift/Track.hpp"

namespace Chicane
{
    namespace Grid
    {
        static bool areNear(const std::vector<float>& inLeft, const float* inRight, std::uint8_t inArity)
        {
            if (inLeft.size() != static_cast<std::size_t>(inArity))
            {
                return false;
            }

            for (std::uint8_t i = 0; i < inArity; i++)
            {
                if (std::fabs(inLeft.at(i) - inRight[i]) > 0.001f)
                {
                    return false;
                }
            }

            return true;
        }

        StyleAnimator::StyleAnimator()
            : m_active({}),
              m_slots({}),
              m_tweens({}),
              m_player({}),
              m_playerTracks({}),
              m_bHasPlayer(false),
              m_dirty(StylePropertyDirty::None)
        {
            m_slots.fill(INVALID_SLOT);
        }

        void StyleAnimator::tick(float inDeltaTime)
        {
            for (StyleAnimatorTween& entry : m_tweens)
            {
                entry.tween.tick(inDeltaTime);
            }

            for (std::size_t i = m_tweens.size(); i > 0; i--)
            {
                const std::size_t index = i - 1;

                if (!m_tweens.at(index).tween.isFinished())
                {
                    continue;
                }

                stopTween(m_tweens.at(index).id);
            }

            if (m_bHasPlayer)
            {
                m_player.tick(inDeltaTime);

                if (m_player.isFinished() && !m_player.fillsForwards())
                {
                    stopPlayer();
                }
            }
        }

        void StyleAnimator::tween(
            StylePropertyId           inId,
            const float*              inFrom,
            const float*              inTo,
            float                     inDuration,
            float                     inDelay,
            const Drift::EasingCurve& inEasing
        )
        {
            const StylePropertyEntry& entry = StylePropertyTable::get(inId);
            const std::size_t         slot  = m_slots.at(static_cast<std::size_t>(inId));

            const std::vector<float> to(inTo, inTo + entry.arity);

            if (slot != INVALID_SLOT)
            {
                Drift::Tween& existing = m_tweens.at(slot).tween;

                if (areNear(existing.getTo(), inTo, entry.arity) && existing.isPlaying())
                {
                    return;
                }

                existing.setFrom(
                    existing.hasStarted() ? existing.getValue() : std::vector<float>(inFrom, inFrom + entry.arity)
                );
                existing.setTo(to);
                existing.setDuration(inDuration);
                existing.setDelay(inDelay);
                existing.setEasing(inEasing);
                existing.reset();
                existing.play();

                return;
            }

            StyleAnimatorTween created;
            created.id = inId;
            created.tween.setFrom(std::vector<float>(inFrom, inFrom + entry.arity));
            created.tween.setTo(to);
            created.tween.setDuration(inDuration);
            created.tween.setDelay(inDelay);
            created.tween.setEasing(inEasing);
            created.tween.reset();
            created.tween.play();

            m_slots.at(static_cast<std::size_t>(inId)) = m_tweens.size();
            m_tweens.push_back(created);
            m_active.set(static_cast<std::size_t>(inId));

            refreshDirty();
        }

        bool StyleAnimator::hasTween(StylePropertyId inId) const
        {
            return m_slots.at(static_cast<std::size_t>(inId)) != INVALID_SLOT;
        }

        bool StyleAnimator::readTween(StylePropertyId inId, float* outValues) const
        {
            const std::size_t slot = m_slots.at(static_cast<std::size_t>(inId));

            if (slot == INVALID_SLOT)
            {
                return false;
            }

            const std::vector<float> value = m_tweens.at(slot).tween.getValue();
            const std::uint8_t       arity = StylePropertyTable::get(inId).arity;

            if (value.size() < static_cast<std::size_t>(arity))
            {
                return false;
            }

            for (std::uint8_t i = 0; i < arity; i++)
            {
                outValues[i] = value.at(i);
            }

            return true;
        }

        void StyleAnimator::stopTween(StylePropertyId inId)
        {
            const std::size_t slot = m_slots.at(static_cast<std::size_t>(inId));

            if (slot == INVALID_SLOT)
            {
                return;
            }

            const std::size_t last = m_tweens.size() - 1;

            if (slot != last)
            {
                m_tweens.at(slot)                                          = m_tweens.at(last);
                m_slots.at(static_cast<std::size_t>(m_tweens.at(slot).id)) = slot;
            }

            m_tweens.pop_back();
            m_slots.at(static_cast<std::size_t>(inId)) = INVALID_SLOT;
            m_active.reset(static_cast<std::size_t>(inId));

            refreshDirty();
        }

        const std::vector<StyleAnimatorTween>& StyleAnimator::getTweens() const
        {
            return m_tweens;
        }

        void StyleAnimator::stopTweens()
        {
            m_tweens.clear();
            m_slots.fill(INVALID_SLOT);
            m_active.reset();

            refreshDirty();
        }

        void StyleAnimator::play(const Drift::Clip& inClip)
        {
            m_player.setClip(inClip);
            m_player.reset();
            m_player.play();
            m_bHasPlayer = true;

            m_playerTracks.clear();

            for (const Drift::Track& track : inClip.tracks)
            {
                StylePropertyId id = StylePropertyId::Count;

                if (!StylePropertyTable::find(track.name, id))
                {
                    continue;
                }

                StyleAnimatorTrack resolved;
                resolved.id   = id;
                resolved.name = track.name;

                m_playerTracks.push_back(resolved);
            }

            refreshDirty();
        }

        bool StyleAnimator::hasPlayer() const
        {
            return m_bHasPlayer;
        }

        Drift::Player* StyleAnimator::getPlayer()
        {
            return m_bHasPlayer ? &m_player : nullptr;
        }

        const Drift::Player* StyleAnimator::getPlayer() const
        {
            return m_bHasPlayer ? &m_player : nullptr;
        }

        void StyleAnimator::stopPlayer()
        {
            m_bHasPlayer = false;
            m_player     = {};
            m_playerTracks.clear();

            refreshDirty();
        }

        const std::vector<StyleAnimatorTrack>& StyleAnimator::getPlayerTracks() const
        {
            return m_playerTracks;
        }

        void StyleAnimator::stop()
        {
            stopTweens();
            stopPlayer();
        }

        bool StyleAnimator::isIdle() const
        {
            return m_tweens.empty() && !m_bHasPlayer;
        }

        StylePropertyDirty StyleAnimator::getDirty() const
        {
            return m_dirty;
        }

        void StyleAnimator::refreshDirty()
        {
            m_dirty = StylePropertyDirty::None;

            for (const StyleAnimatorTween& entry : m_tweens)
            {
                m_dirty |= StylePropertyTable::get(entry.id).dirty;
            }

            for (const StyleAnimatorTrack& track : m_playerTracks)
            {
                m_dirty |= StylePropertyTable::get(track.id).dirty;
            }
        }
    }
}
