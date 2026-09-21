#include "Chicane/Grid/Component/Input/Slider/Button.reflected.hpp"

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
        InputButtonSlider::InputButtonSlider(const XmlNode& inNode)
            : Container(inNode),
              value(0.0f),
              min(0.0f),
              max(100.0f),
              step(0.0f),
              percentage(0.0f),
              precision(3),
              label(String::empty()),
              valueText(String::empty()),
              m_bIsSliding(false),
              m_bIsEdited(false)
        {
            load(
                "Assets/Engine/UI/Components/Input/Slider/Button.grid",
                "Assets/Engine/UI/Components/Input/Slider/Button.decal"
            );
        }

        bool InputButtonSlider::isFocusable() const
        {
            return true;
        }

        bool InputButtonSlider::onEvent(const WindowEvent& inEvent)
        {
            if (inEvent.type == WindowEventType::MouseButtonUp)
            {
                if (!m_bIsSliding)
                {
                    return false;
                }

                m_bIsSliding = false;

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
                if (SliderMath::isDescendantId(this, hit, "sliderDec") ||
                    SliderMath::isDescendantId(this, hit, "sliderInc") ||
                    SliderMath::isDescendantId(this, hit, "sliderValue"))
                {
                    return false;
                }

                Component* track = SliderMath::findChildId(this, "sliderTrack");
                if (track && !track->containsPoint(event.location))
                {
                    return false;
                }

                m_bIsSliding = true;
                applyAt(event.location);

                return true;
            }

            if (inEvent.type == WindowEventType::MouseMotion)
            {
                if (!m_bIsSliding || !inEvent.data)
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
                setValue(min);

                return true;

            case Input::KeyboardButton::End:
                setValue(max);

                return true;

            default:
                return false;
            }
        }

        void InputButtonSlider::onTick(float inDeltaTime)
        {
            Container::onTick(inDeltaTime);

            refreshLabel();
            refreshRange();
        }

        void InputButtonSlider::setValue(float inValue)
        {
            const float next = SliderMath::snapValue(inValue, min, max, step);
            if (std::fabs(value - next) <= 0.0001f)
            {
                refreshPercentage();
                refreshValueText();

                return;
            }

            value       = next;
            m_bIsEdited = true;
            refreshPercentage();
            refreshValueText();
            commit();
            emitInput();
        }

        void InputButtonSlider::decrement()
        {
            nudge(-1);
        }

        void InputButtonSlider::increment()
        {
            nudge(1);
        }

        void InputButtonSlider::commitText()
        {
            setValue(SliderMath::parseNumber(valueText, value));
        }

        void InputButtonSlider::refreshRange()
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

            const String raw = getAttribute(VALUE_ATTRIBUTE_NAME);
            if (!raw.isEmpty() && (isReference(raw) || !m_bIsEdited))
            {
                value = SliderMath::snapValue(
                    SliderMath::parseNumber(parseText(raw), value), min, max, step
                );
            }
            else
            {
                value = SliderMath::snapValue(value, min, max, step);
            }

            refreshPercentage();
            refreshValueText();
        }

        void InputButtonSlider::refreshLabel()
        {
            const String next = parseText(getAttribute(LABEL_ATTRIBUTE_NAME)).trim();
            if (!next.equals(label))
            {
                label = next;
            }
        }

        void InputButtonSlider::applyAt(const Vec2& inLocation)
        {
            Component*     track = SliderMath::findChildId(this, "sliderTrack");
            const Bounds2D box   = track ? track->getDrawBounds() : getDrawBounds();
            setValue(SliderMath::fromLocation(inLocation, box, min, max, step));
        }

        void InputButtonSlider::nudge(int inSteps, float inScale)
        {
            setValue(value + SliderMath::stepSize(step, min, max) * static_cast<float>(inSteps) * inScale);
        }

        void InputButtonSlider::commit()
        {
            SliderMath::commitFloat(this, VALUE_ATTRIBUTE_NAME, value);
        }

        void InputButtonSlider::emitInput()
        {
            getMethod(getAttribute(ON_INPUT_ATTRIBUTE_NAME)).invoke();
        }

        void InputButtonSlider::refreshPercentage()
        {
            percentage = SliderMath::percent(value, min, max);
        }

        void InputButtonSlider::refreshValueText()
        {
            if (isEditing())
            {
                return;
            }

            const String next = SliderMath::formatValue(value, step, precision);
            if (!next.equals(valueText))
            {
                valueText = next;
            }
        }

        bool InputButtonSlider::isEditing() const
        {
            const Component* field = SliderMath::findChildId(this, "sliderValue");

            return field && field->isFocused();
        }
    }
}
