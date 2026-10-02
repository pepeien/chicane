#include "Chicane/Grid/Component/Input/Slider/Range.reflected.hpp"

#include <algorithm>
#include <cmath>

#include "Chicane/Core/Input/Keyboard/Button.hpp"
#include "Chicane/Core/Input/Keyboard/Event.hpp"
#include "Chicane/Core/Input/Mouse/Button.hpp"
#include "Chicane/Core/Input/Mouse/Button/Event.hpp"
#include "Chicane/Core/Input/Mouse/Motion/Event.hpp"
#include "Chicane/Core/Input/Mouse/Wheel/Event.hpp"
#include "Chicane/Core/Math/Bounds/2D.hpp"
#include "Chicane/Core/Window/Event/Type.hpp"
#include "Chicane/Core/Xml.hpp"

#include "Chicane/Grid/Component/Input/Slider/Math.hpp"

namespace Chicane
{
    namespace Grid
    {
        InputRangeSlider::InputRangeSlider(const XmlNode& inNode)
            : Container(inNode),
              low(0.0f),
              high(100.0f),
              min(0.0f),
              max(100.0f),
              step(0.0f),
              lowPercent(0.0f),
              spanPercent(100.0f),
              precision(3),
              lowText(String::sEmpty()),
              highText(String::sEmpty()),
              m_drag(InputRangeSliderDrag::None),
              m_spanWidth(0.0f),
              m_grabOffset(0.0f),
              m_bIsEdited(false)
        {
            load(
                "Assets/Engine/UI/Components/Input/Slider/Range/Index.grid",
                "Assets/Engine/UI/Components/Input/Slider/Range/Index.decal"
            );

            setAttribute(FOCUSABLE_ATTRIBUTE_NAME, "true");
        }

        bool InputRangeSlider::onEvent(const WindowEvent& inEvent)
        {
            if (inEvent.type == WindowEventType::MouseButtonUp)
            {
                if (m_drag == InputRangeSliderDrag::None)
                {
                    return false;
                }

                m_drag = InputRangeSliderDrag::None;

                return true;
            }

            if (inEvent.type == WindowEventType::MouseButtonDown)
            {
                if (!inEvent.data)
                {
                    return false;
                }

                const Input::MouseButtonEvent event = *static_cast<Input::MouseButtonEvent*>(inEvent.data);
                if (event.button != Input::MouseButton::Left || !containsPoint(event.location))
                {
                    return false;
                }

                Component* hit = hasRoot() ? getRoot()->getHitAt(event.location) : nullptr;
                if (SliderMath::isDescendantId(this, hit, "rangeLow") ||
                    SliderMath::isDescendantId(this, hit, "rangeHigh"))
                {
                    return false;
                }

                m_drag = hitDrag(event.location);
                if (m_drag == InputRangeSliderDrag::None)
                {
                    return false;
                }

                const bool bDragSpan = static_cast<bool>(m_drag == InputRangeSliderDrag::Span);

                if (bDragSpan)
                {
                    Component*     track   = SliderMath::findChildId(this, "rangeTrack");
                    const Bounds2D box     = track ? track->getDrawBounds() : getDrawBounds();
                    const float    pointer = SliderMath::fromLocation(event.location, box, min, max, 0.0f);
                    m_spanWidth            = high - low;
                    m_grabOffset           = pointer - low;
                }

                if (!bDragSpan)
                {
                    applyAt(event.location);
                }

                return true;
            }

            if (inEvent.type == WindowEventType::MouseMotion)
            {
                if (m_drag == InputRangeSliderDrag::None || !inEvent.data)
                {
                    return false;
                }

                const Input::MouseMotionEvent event = *static_cast<Input::MouseMotionEvent*>(inEvent.data);
                applyAt(event.location);

                return true;
            }

            if (inEvent.type == WindowEventType::MouseWheel)
            {
                if (isEditing() || !inEvent.data)
                {
                    return false;
                }

                const Input::MouseWheelEvent event = *static_cast<Input::MouseWheelEvent*>(inEvent.data);
                if (!containsPoint(event.location))
                {
                    return false;
                }

                const int steps = SliderMath::wheelSteps(event.delta);
                if (steps == 0)
                {
                    return false;
                }

                nudge(steps);

                return true;
            }

            if (!isFocused() || inEvent.type != WindowEventType::KeyDown || !inEvent.data || isEditing())
            {
                return false;
            }

            const Input::KeyboardEvent event = *static_cast<Input::KeyboardEvent*>(inEvent.data);
            const float                scale = SliderMath::modifierScale(event.modifier);
            switch (event.button)
            {
            case Input::KeyboardButton::Left:
            case Input::KeyboardButton::Down:
                nudge(-1, scale);

                return true;

            case Input::KeyboardButton::Right:
            case Input::KeyboardButton::Up:
                nudge(1, scale);

                return true;

            case Input::KeyboardButton::Home:
                m_spanWidth = high - low;
                applyRange(min, min + m_spanWidth, InputRangeSliderDrag::Span);

                return true;

            case Input::KeyboardButton::End:
                m_spanWidth = high - low;
                applyRange(max - m_spanWidth, max, InputRangeSliderDrag::Span);

                return true;

            default:
                return false;
            }
        }

        void InputRangeSlider::onTick(float inDeltaTime)
        {
            Container::onTick(inDeltaTime);

            refreshRange();
        }

        void InputRangeSlider::setLow(float inValue)
        {
            applyRange(inValue, high, InputRangeSliderDrag::Low);
        }

        void InputRangeSlider::setHigh(float inValue)
        {
            applyRange(low, inValue, InputRangeSliderDrag::High);
        }

        void InputRangeSlider::commitLowText()
        {
            setLow(SliderMath::parseNumber(lowText, low));
        }

        void InputRangeSlider::commitHighText()
        {
            setHigh(SliderMath::parseNumber(highText, high));
        }

        void InputRangeSlider::refreshRange()
        {
            min       = SliderMath::parseNumber(parseText(getAttribute(MIN_ATTRIBUTE_NAME)), 0.0f);
            max       = SliderMath::parseNumber(parseText(getAttribute(MAX_ATTRIBUTE_NAME)), 100.0f);
            step      = std::max(0.0f, SliderMath::parseNumber(parseText(getAttribute(STEP_ATTRIBUTE_NAME)), 0.0f));
            precision = std::clamp(
                static_cast<int>(SliderMath::parseNumber(parseText(getAttribute(PRECISION_ATTRIBUTE_NAME)), 3.0f)),
                0,
                6
            );

            if (max < min)
            {
                std::swap(min, max);
            }

            const String rawLow   = getAttribute(LOW_ATTRIBUTE_NAME);
            const String rawHigh  = getAttribute(HIGH_ATTRIBUTE_NAME);
            float        nextLow  = low;
            float        nextHigh = high;

            if (!rawLow.isEmpty() && (isReference(rawLow) || !m_bIsEdited))
            {
                nextLow = SliderMath::parseNumber(parseText(rawLow), nextLow);
            }

            if (!rawHigh.isEmpty() && (isReference(rawHigh) || !m_bIsEdited))
            {
                nextHigh = SliderMath::parseNumber(parseText(rawHigh), nextHigh);
            }

            nextLow  = SliderMath::snapValue(nextLow, min, max, step);
            nextHigh = SliderMath::snapValue(nextHigh, min, max, step);
            if (nextHigh < nextLow)
            {
                std::swap(nextLow, nextHigh);
            }

            const float minimum = gap();
            if (nextHigh - nextLow < minimum)
            {
                nextHigh = SliderMath::snapValue(nextLow + minimum, min, max, step);
                if (nextHigh - nextLow < minimum)
                {
                    nextLow = SliderMath::snapValue(nextHigh - minimum, min, max, step);
                }
            }

            low  = nextLow;
            high = nextHigh;
            refreshPercents();
            refreshValueText();
        }

        void InputRangeSlider::applyAt(const Vec2& inLocation)
        {
            Component*     track = SliderMath::findChildId(this, "rangeTrack");
            const Bounds2D box   = track ? track->getDrawBounds() : getDrawBounds();

            if (m_drag == InputRangeSliderDrag::Span)
            {
                const float pointer = SliderMath::fromLocation(inLocation, box, min, max, 0.0f);
                applyRange(pointer - m_grabOffset, pointer - m_grabOffset + m_spanWidth, InputRangeSliderDrag::Span);

                return;
            }

            const float next = SliderMath::fromLocation(inLocation, box, min, max, step);
            if (m_drag == InputRangeSliderDrag::High)
            {
                setHigh(next);

                return;
            }

            setLow(next);
        }

        void InputRangeSlider::applyRange(float inLow, float inHigh, InputRangeSliderDrag inMode)
        {
            float nextLow  = SliderMath::snapValue(inLow, min, max, step);
            float nextHigh = SliderMath::snapValue(inHigh, min, max, step);
            if (nextHigh < nextLow)
            {
                std::swap(nextLow, nextHigh);
            }

            const float minimum = gap();
            const bool  bModeSpan  = static_cast<bool>(inMode == InputRangeSliderDrag::Span);

            if (bModeSpan)
            {
                const float width = std::max(minimum, m_spanWidth);
                nextLow           = SliderMath::clampValue(nextLow, min, max - width);
                nextHigh          = SliderMath::snapValue(nextLow + width, min, max, step);
                nextLow           = SliderMath::snapValue(nextHigh - width, min, max, step);
            }

            const bool bModeHigh = !bModeSpan && (inMode == InputRangeSliderDrag::High);

            if (bModeHigh)
            {
                nextHigh = std::max(nextHigh, SliderMath::snapValue(low + minimum, min, max, step));
                nextLow  = low;
            }

            const bool bModeLow = !bModeSpan && !bModeHigh && (inMode == InputRangeSliderDrag::Low);

            if (bModeLow)
            {
                nextLow  = std::min(nextLow, SliderMath::snapValue(high - minimum, min, max, step));
                nextHigh = high;
            }

            const bool bNextLowMinimum = !bModeSpan && !bModeHigh && !bModeLow && (nextHigh - nextLow < minimum);

            if (bNextLowMinimum)
            {
                nextHigh = SliderMath::snapValue(nextLow + minimum, min, max, step);
            }

            if (std::fabs(low - nextLow) <= 0.0001f && std::fabs(high - nextHigh) <= 0.0001f)
            {
                refreshPercents();
                refreshValueText();

                return;
            }

            low         = nextLow;
            high        = nextHigh;
            m_bIsEdited = true;
            refreshPercents();
            refreshValueText();
            commit();
            emitInput();
        }

        void InputRangeSlider::nudge(int inSteps, float inScale)
        {
            const float amount = SliderMath::stepSize(step, min, max) * static_cast<float>(inSteps) * inScale;
            m_spanWidth        = high - low;
            applyRange(low + amount, high + amount, InputRangeSliderDrag::Span);
        }

        void InputRangeSlider::commit()
        {
            SliderMath::commitFloat(this, LOW_ATTRIBUTE_NAME, low);
            SliderMath::commitFloat(this, HIGH_ATTRIBUTE_NAME, high);
        }

        void InputRangeSlider::emitInput()
        {
            getMethod(getAttribute(ON_INPUT_ATTRIBUTE_NAME)).invoke();
        }

        void InputRangeSlider::refreshPercents()
        {
            lowPercent  = SliderMath::percent(low, min, max);
            spanPercent = std::max(0.0f, SliderMath::percent(high, min, max) - lowPercent);
        }

        void InputRangeSlider::refreshValueText()
        {
            if (!isEditingLow())
            {
                const String next = SliderMath::formatValue(low, step, precision);
                if (!next.equals(lowText))
                {
                    lowText = next;
                }
            }

            if (!isEditingHigh())
            {
                const String next = SliderMath::formatValue(high, step, precision);
                if (!next.equals(highText))
                {
                    highText = next;
                }
            }
        }

        InputRangeSliderDrag InputRangeSlider::hitDrag(const Vec2& inLocation) const
        {
            const Component* handleLow  = SliderMath::findChildId(this, "rangeHandleLow");
            const Component* handleHigh = SliderMath::findChildId(this, "rangeHandleHigh");
            const Component* span       = SliderMath::findChildId(this, "rangeSpan");
            const Component* track      = SliderMath::findChildId(this, "rangeTrack");

            const bool bHitLow  = handleLow && handleLow->containsPoint(inLocation);
            const bool bHitHigh = handleHigh && handleHigh->containsPoint(inLocation);
            if (bHitLow && bHitHigh)
            {
                const Bounds2D lowBox  = handleLow->getDrawBounds();
                const Bounds2D highBox = handleHigh->getDrawBounds();
                const float    lowMid  = (lowBox.left + lowBox.right) * 0.5f;
                const float    highMid = (highBox.left + highBox.right) * 0.5f;

                return std::fabs(inLocation.x - lowMid) <= std::fabs(inLocation.x - highMid)
                           ? InputRangeSliderDrag::Low
                           : InputRangeSliderDrag::High;
            }

            if (bHitLow)
            {
                return InputRangeSliderDrag::Low;
            }

            if (bHitHigh)
            {
                return InputRangeSliderDrag::High;
            }

            if (span && span->containsPoint(inLocation))
            {
                return InputRangeSliderDrag::Span;
            }

            if (track && !track->containsPoint(inLocation))
            {
                return InputRangeSliderDrag::None;
            }

            const Bounds2D box  = track ? track->getDrawBounds() : getDrawBounds();
            const float    next = SliderMath::fromLocation(inLocation, box, min, max, 0.0f);

            return std::fabs(next - low) <= std::fabs(next - high) ? InputRangeSliderDrag::Low
                                                                   : InputRangeSliderDrag::High;
        }

        bool InputRangeSlider::isEditing() const
        {
            return isEditingLow() || isEditingHigh();
        }

        bool InputRangeSlider::isEditingLow() const
        {
            const Component* field = SliderMath::findChildId(this, "rangeLow");

            return field && field->isFocused();
        }

        bool InputRangeSlider::isEditingHigh() const
        {
            const Component* field = SliderMath::findChildId(this, "rangeHigh");

            return field && field->isFocused();
        }

        float InputRangeSlider::gap() const
        {
            return SliderMath::stepSize(step, min, max);
        }
    }
}
