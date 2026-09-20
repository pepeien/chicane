#include "Chicane/Grid/Animatable.reflected.hpp"

#include <cmath>

#include "Chicane/Drift/Direction.hpp"

namespace Chicane
{
    namespace Grid
    {
        static bool areNear(const float* inLeft, const float* inRight, std::uint8_t inArity)
        {
            for (std::uint8_t i = 0; i < inArity; i++)
            {
                if (std::fabs(inLeft[i] - inRight[i]) > 0.001f)
                {
                    return false;
                }
            }

            return true;
        }

        Animatable::Animatable()
            : Transformable2D(),
              m_animator(),
              m_animationClip(String::empty()),
              m_lastTransformRaw(String::empty()),
              m_lastTranslateRaw(String::empty()),
              m_animationDelta(0.0f),
              m_bIsAnimationReady(false)
        {}

        void Animatable::tickAnimation(Style& outStyle, float inDeltaTime)
        {
            auto rememberResolved = [&outStyle, this]()
            {
                m_lastTransformRaw = outStyle.transform.getRaw();
                m_lastTranslateRaw = outStyle.translate.getRaw();
            };

            if (!m_bIsAnimationReady)
            {
                outStyle.snapshot();
                m_bIsAnimationReady = true;
                rememberResolved();

                return;
            }

            if (!canPlayAnimation())
            {
                m_animator.stop();
                m_animationClip = "";
                outStyle.snapshot();
                rememberResolved();

                return;
            }

            const Style::AnimatedValues& visual     = outStyle.getSnapshotValues();
            const Style::AnimatedMask&   visualMask = outStyle.getSnapshotMask();

            float target[MAX_PROPERTY_ARITY] = {};

            for (std::size_t i = 0; i < StylePropertyTable::COUNT; i++)
            {
                const StylePropertyId id = static_cast<StylePropertyId>(i);

                if (!outStyle.readAnimated(id, target))
                {
                    m_animator.stopTween(id);

                    continue;
                }

                const StyleTransition* transition = outStyle.findTransition(id);

                if (!transition || transition->duration <= 0.0f)
                {
                    m_animator.stopTween(id);

                    continue;
                }

                const std::uint8_t arity = StylePropertyTable::get(id).arity;
                const float* from = visualMask.test(i) ? (visual.data() + StylePropertyTable::offset(id)) : target;

                if (areNear(from, target, arity))
                {
                    continue;
                }

                const bool bHasPercentBasis =
                    (id == StylePropertyId::Transform && outStyle.transform.getRaw().contains('%') &&
                     outStyle.transform.getRaw().equals(m_lastTransformRaw)) ||
                    (id == StylePropertyId::Translate && outStyle.translate.getRaw().contains('%') &&
                     outStyle.translate.getRaw().equals(m_lastTranslateRaw));

                if (bHasPercentBasis && !m_animator.hasTween(id))
                {
                    continue;
                }

                m_animator.tween(id, from, target, transition->duration, transition->delay, transition->easing);
            }

            const String animationName = outStyle.animation.name.trim();

            if (animationName.isEmpty() || animationName.equals(Style::ANIMATION_NAME_NONE))
            {
                m_animator.stopPlayer();
                m_animationClip = "";
            }
            else if (!m_animationClip.equals(animationName) || !m_animator.hasPlayer())
            {
                const StyleKeyframe::List* frames = findKeyframes(animationName);

                if (frames && !frames->empty() && outStyle.animation.duration > 0.0f)
                {
                    m_animator.play(makeAnimationClip(*frames));

                    if (Drift::Player* player = m_animator.getPlayer())
                    {
                        player->setDelay(outStyle.animation.delay);
                        player->setDirection(
                            outStyle.animation.bIsReverse ? Drift::Direction::Reverse : Drift::Direction::Forward
                        );
                        player->setFillForwards(outStyle.animation.bShouldFillForwards);
                        player->setFillBackwards(outStyle.animation.bShouldFillBackwards);

                        if (outStyle.animation.bIsPaused)
                        {
                            player->pause();
                        }
                    }

                    m_animationClip = animationName;
                }
            }
            else if (Drift::Player* player = m_animator.getPlayer())
            {
                if (outStyle.animation.bIsPaused)
                {
                    player->pause();
                }
                else if (player->isPaused())
                {
                    player->play();
                }
            }

            m_animator.tick(inDeltaTime);

            float sampled[MAX_PROPERTY_ARITY] = {};

            for (const StyleAnimatorTween& entry : m_animator.getTweens())
            {
                if (m_animator.readTween(entry.id, sampled))
                {
                    outStyle.writeAnimated(entry.id, sampled);
                }
            }

            if (const Drift::Player* player = m_animator.getPlayer())
            {
                const bool bIsWaiting = player->isWaiting();
                const bool bIsActive  = player->isPlaying() || player->isPaused();
                const bool bIsFilled  = player->isFinished() && player->fillsForwards();
                const bool bIsBack    = bIsWaiting && player->fillsBackwards();

                if ((!bIsWaiting && (bIsActive || bIsFilled)) || bIsBack)
                {
                    for (const StyleAnimatorTrack& track : m_animator.getPlayerTracks())
                    {
                        const std::vector<float> value = player->sample(track.name);
                        const std::uint8_t       arity = StylePropertyTable::get(track.id).arity;

                        if (value.size() < static_cast<std::size_t>(arity))
                        {
                            continue;
                        }

                        outStyle.writeAnimated(track.id, value.data());
                    }
                }
            }

            outStyle.snapshot();
            rememberResolved();
        }
    }
}
