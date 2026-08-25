#include "Chicane/Grid/Animatable.reflected.hpp"

#include <cmath>

#include "Chicane/Drift/Direction.hpp"

namespace Chicane
{
    namespace Grid
    {
        Animatable::Animatable()
            : Transformable2D(),
              m_animator(),
              m_animationClip(String::empty()),
              m_animationDelta(0.0f),
              m_bIsAnimationReady(false)
        {}

        void Animatable::tickAnimation(Style& outStyle, float inDeltaTime)
        {
            const Style::Properties targets = outStyle.extractAnimatedProperties();

            if (!m_bIsAnimationReady)
            {
                outStyle.snapshot();
                m_bIsAnimationReady = true;

                return;
            }

            if (!canPlayAnimation())
            {
                m_animator.stop();
                m_animationClip = "";
                outStyle.snapshot();

                return;
            }

            auto sameValues = [](const std::vector<float>& inLeft, const std::vector<float>& inRight) -> bool
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
            };

            const Style::Properties& visual = outStyle.getSnapshot();

            for (const String& name : Style::ANIMATABLE_PROPERTIES)
            {
                const auto               targetFound = targets.find(name);
                const std::vector<float> to = targetFound != targets.end() ? targetFound->second : std::vector<float>();

                if (to.empty())
                {
                    m_animator.stopTween(name);

                    continue;
                }

                const auto               visualFound = visual.find(name);
                const std::vector<float> from =
                    visualFound != visual.end() && !visualFound->second.empty() ? visualFound->second : to;

                const StyleTransition* transition = outStyle.findTransition(name);

                if (!transition || transition->duration <= 0.0f)
                {
                    m_animator.stopTween(name);

                    continue;
                }

                if (sameValues(from, to))
                {
                    m_animator.stopTween(name);

                    continue;
                }

                m_animator.tween(name, from, to, transition->duration, transition->delay, transition->easing);
            }

            const String animationName = outStyle.animation.name.trim();

            if (animationName.isEmpty() || animationName.equals(Style::ANIMATION_NAME_NONE))
            {
                m_animator.stopPlayer("style");
                m_animationClip = "";
            }
            else if (!m_animationClip.equals(animationName) || !m_animator.hasPlayer("style"))
            {
                const StyleKeyframe::List* frames = findKeyframes(animationName);

                if (frames && !frames->empty() && outStyle.animation.duration > 0.0f)
                {
                    const Drift::Clip clip = makeAnimationClip(*frames);

                    m_animator.play("style", clip);

                    if (Drift::Player* player = m_animator.getPlayer("style"))
                    {
                        player->setDelay(outStyle.animation.delay);
                        player->setDirection(
                            outStyle.animation.bReverse ? Drift::Direction::Reverse : Drift::Direction::Forward
                        );
                        player->setFillForwards(outStyle.animation.bFillForwards);
                        player->setFillBackwards(outStyle.animation.bFillBackwards);

                        if (outStyle.animation.bPaused)
                        {
                            player->pause();
                        }
                    }

                    m_animationClip = animationName;
                }
            }
            else if (Drift::Player* player = m_animator.getPlayer("style"))
            {
                if (outStyle.animation.bPaused)
                {
                    player->pause();
                }
                else if (player->isPaused())
                {
                    player->play();
                }
            }

            m_animator.tick(inDeltaTime);

            for (const String& name : Style::ANIMATABLE_PROPERTIES)
            {
                if (!m_animator.hasTween(name))
                {
                    continue;
                }

                outStyle.applyAnimatedProperty(name, m_animator.getTweenValue(name));
            }

            if (const Drift::Player* player = m_animator.getPlayer("style"))
            {
                const bool bWaiting = player->isWaiting();
                const bool bActive  = player->isPlaying() || player->isPaused();
                const bool bFilled  = player->isFinished() && player->fillsForwards();
                const bool bBack    = bWaiting && player->fillsBackwards();

                if ((!bWaiting && (bActive || bFilled)) || bBack)
                {
                    for (const Drift::Track& track : player->getClip().tracks)
                    {
                        const std::vector<float> value = player->sample(track.name);

                        if (value.empty())
                        {
                            continue;
                        }

                        outStyle.applyAnimatedProperty(track.name, value);
                    }
                }
            }

            outStyle.snapshot();
        }
    }
}
