#include "Chicane/Grid/Animatable.reflected.hpp"

#include <algorithm>
#include <cmath>

#include "Chicane/Core/Color.hpp"
#include "Chicane/Core/Size.hpp"

#include "Chicane/Drift/Direction.hpp"
#include "Chicane/Drift/Loop.hpp"

namespace Chicane
{
    namespace Grid
    {
        Animatable::Animatable()
            : Transformable2D(),
              m_animator(),
              m_animationVisual({}),
              m_animationTargets({}),
              m_animationClip(String::empty()),
              m_animationDelta(0.0f),
              m_bIsAnimationReady(false)
        {}

        void Animatable::snapshotAnimationVisual(const Style& inStyle)
        {
            if (!m_bIsAnimationReady)
            {
                return;
            }

            m_animationVisual = extractAnimatedProperties(inStyle);
        }

        void Animatable::restoreAnimationVisual(
            Style& outStyle, const std::unordered_map<String, std::vector<float>>& inVisual
        )
        {
            if (!m_bIsAnimationReady)
            {
                return;
            }

            m_animationVisual = inVisual;

            for (const auto& [name, value] : inVisual)
            {
                if (value.empty())
                {
                    continue;
                }

                const StyleTransition* transition = findTransition(outStyle, name);

                if (!transition || transition->duration <= 0.0f)
                {
                    continue;
                }

                applyAnimatedProperty(outStyle, name, value);
            }
        }

        void Animatable::tickAnimation(Style& outStyle, float inDeltaTime)
        {
            const std::unordered_map<String, std::vector<float>> targets = extractAnimatedProperties(outStyle);

            if (!m_bIsAnimationReady)
            {
                m_animationTargets  = targets;
                m_animationVisual   = targets;
                m_bIsAnimationReady = true;

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

            for (const String& name : Style::ANIMATABLE_PROPERTIES)
            {
                const auto               targetFound = targets.find(name);
                const std::vector<float> to = targetFound != targets.end() ? targetFound->second : std::vector<float>();

                if (to.empty())
                {
                    m_animator.stopTween(name);

                    continue;
                }

                const auto               visualFound = m_animationVisual.find(name);
                const std::vector<float> from =
                    visualFound != m_animationVisual.end() && !visualFound->second.empty() ? visualFound->second : to;

                const StyleTransition* transition = findTransition(outStyle, name);

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

                applyAnimatedProperty(outStyle, name, m_animator.getTweenValue(name));
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

                        applyAnimatedProperty(outStyle, track.name, value);
                    }
                }
            }

            m_animationVisual  = extractAnimatedProperties(outStyle);
            m_animationTargets = targets;
        }

        Animatable::Properties Animatable::extractAnimatedProperties(const Style& inStyle) const
        {
            Properties result;

            for (const String& name : Style::ANIMATABLE_PROPERTIES)
            {
                const std::vector<float> values = extractAnimatedProperty(inStyle, name);

                if (values.empty())
                {
                    continue;
                }

                result[name] = values;
            }

            return result;
        }

        std::vector<float> Animatable::extractAnimatedProperty(const Style& inStyle, const String& inName) const
        {
            auto asColor = [](const Color::Rgba& inValue) -> std::vector<float>
            {
                return {
                    static_cast<float>(inValue.r),
                    static_cast<float>(inValue.g),
                    static_cast<float>(inValue.b),
                    static_cast<float>(inValue.a)
                };
            };

            if (inName.equals(Style::OPACITY_ATTRIBUTE_NAME))
            {
                return {inStyle.opacity.get()};
            }

            if (inName.equals(Style::WIDTH_ATTRIBUTE_NAME))
            {
                if (inStyle.width.getRaw().isEmpty() || inStyle.width.isRaw(Size::AUTO_KEYWORD))
                {
                    return {};
                }

                return {inStyle.width.get()};
            }

            if (inName.equals(Style::HEIGHT_ATTRIBUTE_NAME))
            {
                if (inStyle.height.getRaw().isEmpty() || inStyle.height.isRaw(Size::AUTO_KEYWORD))
                {
                    return {};
                }

                return {inStyle.height.get()};
            }

            if (inName.equals(Style::Z_INDEX_ATTRIBUTE_NAME))
            {
                return {inStyle.zIndex.get()};
            }

            if (inName.equals(Style::FOREGROUND_COLOR_ATTRIBUTE_NAME))
            {
                return asColor(inStyle.foregroundColor.get());
            }

            if (inName.equals(Style::BACKGROUND_COLOR_ATTRIBUTE_NAME))
            {
                return asColor(inStyle.background.color.get());
            }

            if (inName.equals(Style::FONT_SIZE_ATTRIBUTE_NAME))
            {
                return {inStyle.font.size.get()};
            }

            if (inName.equals(Style::LETTER_SPACING_ATTRIBUTE_NAME))
            {
                return {inStyle.letterSpacing.get()};
            }

            if (inName.equals(Style::FILTER_ATTRIBUTE_NAME))
            {
                return {inStyle.filter.blur.get()};
            }

            if (inName.equals(Style::BACKDROP_FILTER_ATTRIBUTE_NAME))
            {
                return {inStyle.backdrop.blur.get()};
            }

            if (inName.equals(Style::MARGIN_TOP_ATTRIBUTE_NAME))
            {
                return {inStyle.margin.top.get()};
            }

            if (inName.equals(Style::MARGIN_BOTTOM_ATTRIBUTE_NAME))
            {
                return {inStyle.margin.bottom.get()};
            }

            if (inName.equals(Style::MARGIN_LEFT_ATTRIBUTE_NAME))
            {
                return {inStyle.margin.left.get()};
            }

            if (inName.equals(Style::MARGIN_RIGHT_ATTRIBUTE_NAME))
            {
                return {inStyle.margin.right.get()};
            }

            if (inName.equals(Style::PADDING_TOP_ATTRIBUTE_NAME))
            {
                return {inStyle.padding.top.get()};
            }

            if (inName.equals(Style::PADDING_BOTTOM_ATTRIBUTE_NAME))
            {
                return {inStyle.padding.bottom.get()};
            }

            if (inName.equals(Style::PADDING_LEFT_ATTRIBUTE_NAME))
            {
                return {inStyle.padding.left.get()};
            }

            if (inName.equals(Style::PADDING_RIGHT_ATTRIBUTE_NAME))
            {
                return {inStyle.padding.right.get()};
            }

            if (inName.equals(Style::GAP_TOP_ATTRIBUTE_NAME))
            {
                return {inStyle.gap.top.get()};
            }

            if (inName.equals(Style::GAP_BOTTOM_ATTRIBUTE_NAME))
            {
                return {inStyle.gap.bottom.get()};
            }

            if (inName.equals(Style::GAP_LEFT_ATTRIBUTE_NAME))
            {
                return {inStyle.gap.left.get()};
            }

            if (inName.equals(Style::GAP_RIGHT_ATTRIBUTE_NAME))
            {
                return {inStyle.gap.right.get()};
            }

            return {};
        }

        void Animatable::applyAnimatedProperty(Style& outStyle, const String& inName, const std::vector<float>& inValue)
        {
            if (inValue.empty())
            {
                return;
            }

            auto asColor = [](const std::vector<float>& inChannels) -> Color::Rgba
            {
                auto channel = [](float inChannel) -> std::uint8_t
                { return static_cast<std::uint8_t>(std::round(std::clamp(inChannel, 0.0f, 255.0f))); };

                if (inChannels.size() < 4)
                {
                    return Color::Rgba(0U, 0U, 0U, 0U);
                }

                return Color::Rgba(
                    channel(inChannels.at(0)),
                    channel(inChannels.at(1)),
                    channel(inChannels.at(2)),
                    channel(inChannels.at(3))
                );
            };

            if (inName.equals(Style::OPACITY_ATTRIBUTE_NAME))
            {
                outStyle.opacity.set(inValue.at(0));

                return;
            }

            if (inName.equals(Style::WIDTH_ATTRIBUTE_NAME))
            {
                outStyle.width.set(inValue.at(0));

                return;
            }

            if (inName.equals(Style::HEIGHT_ATTRIBUTE_NAME))
            {
                outStyle.height.set(inValue.at(0));

                return;
            }

            if (inName.equals(Style::Z_INDEX_ATTRIBUTE_NAME))
            {
                outStyle.zIndex.set(inValue.at(0));

                return;
            }

            if (inName.equals(Style::FOREGROUND_COLOR_ATTRIBUTE_NAME))
            {
                outStyle.foregroundColor.set(asColor(inValue));

                return;
            }

            if (inName.equals(Style::BACKGROUND_COLOR_ATTRIBUTE_NAME))
            {
                outStyle.background.color.set(asColor(inValue));

                return;
            }

            if (inName.equals(Style::FONT_SIZE_ATTRIBUTE_NAME))
            {
                outStyle.font.size.set(inValue.at(0));

                return;
            }

            if (inName.equals(Style::LETTER_SPACING_ATTRIBUTE_NAME))
            {
                outStyle.letterSpacing.set(inValue.at(0));

                return;
            }

            if (inName.equals(Style::FILTER_ATTRIBUTE_NAME))
            {
                outStyle.filter.blur.set(inValue.at(0));

                return;
            }

            if (inName.equals(Style::BACKDROP_FILTER_ATTRIBUTE_NAME))
            {
                outStyle.backdrop.blur.set(inValue.at(0));

                return;
            }

            if (inName.equals(Style::MARGIN_TOP_ATTRIBUTE_NAME))
            {
                outStyle.margin.top.set(inValue.at(0));

                return;
            }

            if (inName.equals(Style::MARGIN_BOTTOM_ATTRIBUTE_NAME))
            {
                outStyle.margin.bottom.set(inValue.at(0));

                return;
            }

            if (inName.equals(Style::MARGIN_LEFT_ATTRIBUTE_NAME))
            {
                outStyle.margin.left.set(inValue.at(0));

                return;
            }

            if (inName.equals(Style::MARGIN_RIGHT_ATTRIBUTE_NAME))
            {
                outStyle.margin.right.set(inValue.at(0));

                return;
            }

            if (inName.equals(Style::PADDING_TOP_ATTRIBUTE_NAME))
            {
                outStyle.padding.top.set(inValue.at(0));

                return;
            }

            if (inName.equals(Style::PADDING_BOTTOM_ATTRIBUTE_NAME))
            {
                outStyle.padding.bottom.set(inValue.at(0));

                return;
            }

            if (inName.equals(Style::PADDING_LEFT_ATTRIBUTE_NAME))
            {
                outStyle.padding.left.set(inValue.at(0));

                return;
            }

            if (inName.equals(Style::PADDING_RIGHT_ATTRIBUTE_NAME))
            {
                outStyle.padding.right.set(inValue.at(0));

                return;
            }

            if (inName.equals(Style::GAP_TOP_ATTRIBUTE_NAME))
            {
                outStyle.gap.top.set(inValue.at(0));

                return;
            }

            if (inName.equals(Style::GAP_BOTTOM_ATTRIBUTE_NAME))
            {
                outStyle.gap.bottom.set(inValue.at(0));

                return;
            }

            if (inName.equals(Style::GAP_LEFT_ATTRIBUTE_NAME))
            {
                outStyle.gap.left.set(inValue.at(0));

                return;
            }

            if (inName.equals(Style::GAP_RIGHT_ATTRIBUTE_NAME))
            {
                outStyle.gap.right.set(inValue.at(0));
            }
        }

        const StyleTransition* Animatable::findTransition(const Style& inStyle, const String& inName) const
        {
            const StyleTransition* all      = nullptr;
            const StyleTransition* specific = nullptr;

            for (const StyleTransition& transition : inStyle.transitions)
            {
                if (transition.property.equals(Style::TRANSITION_PROPERTY_ALL))
                {
                    all = &transition;
                }

                if (transition.property.equals(inName))
                {
                    specific = &transition;
                }
            }

            return specific ? specific : all;
        }
    }
}
