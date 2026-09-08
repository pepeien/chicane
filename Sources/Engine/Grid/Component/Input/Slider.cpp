#include "Chicane/Grid/Component/Input/Slider.reflected.hpp"

#include <algorithm>
#include <cmath>
#include <cstdlib>

#include "Chicane/Core/Input/Keyboard/Button.hpp"
#include "Chicane/Core/Input/Keyboard/Event.hpp"
#include "Chicane/Core/Input/Mouse/Button.hpp"
#include "Chicane/Core/Input/Mouse/Button/Event.hpp"
#include "Chicane/Core/Input/Mouse/Motion/Event.hpp"

namespace Chicane
{
    namespace Grid
    {
        InputSlider::InputSlider(const pugi::xml_node& inNode)
            : Container(inNode),
              value(0.0f),
              min(0.0f),
              max(100.0f),
              step(0.0f),
              percentage(0.0f),
              m_bIsSliding(false),
              m_bIsEdited(false)
        {
            load("Assets/Engine/UI/Components/Input/Slider.grid", "Assets/Engine/UI/Components/Input/Slider.decal");
        }

        bool InputSlider::isFocusable() const
        {
            return true;
        }

        bool InputSlider::onEvent(const WindowEvent& inEvent)
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

            if (!isFocused() || inEvent.type != WindowEventType::KeyDown || !inEvent.data)
            {
                return false;
            }

            const Input::KeyboardEvent event = *static_cast<Input::KeyboardEvent*>(inEvent.data);
            switch (event.button)
            {
            case Input::KeyboardButton::Left:
            case Input::KeyboardButton::Down:
                nudge(-1);

                return true;

            case Input::KeyboardButton::Right:
            case Input::KeyboardButton::Up:
                nudge(1);

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

        void InputSlider::onTick(float inDeltaTime)
        {
            Container::onTick(inDeltaTime);

            refreshRange();
        }

        void InputSlider::setValue(float inValue)
        {
            const float next = snapValue(clampValue(inValue));
            if (std::fabs(value - next) <= 0.0001f)
            {
                refreshPercentage();

                return;
            }

            value       = next;
            m_bIsEdited = true;
            refreshPercentage();
            commit();
            emitInput();
        }

        void InputSlider::refreshRange()
        {
            min  = parseNumber(getAttribute(MIN_ATTRIBUTE_NAME), 0.0f);
            max  = parseNumber(getAttribute(MAX_ATTRIBUTE_NAME), 100.0f);
            step = std::max(0.0f, parseNumber(getAttribute(STEP_ATTRIBUTE_NAME), 0.0f));

            if (max < min)
            {
                std::swap(min, max);
            }

            const String raw = getAttribute(VALUE_ATTRIBUTE_NAME);
            if (!raw.isEmpty() && (isReference(raw) || !m_bIsEdited))
            {
                value = snapValue(clampValue(parseNumber(raw, value)));
            }
            else
            {
                value = snapValue(clampValue(value));
            }

            refreshPercentage();
        }

        void InputSlider::applyAt(const Vec2& inLocation)
        {
            const Bounds2D box   = getDrawBounds();
            const float    width = box.right - box.left;
            if (width <= 0.0f)
            {
                return;
            }

            const float t = std::clamp((inLocation.x - box.left) / width, 0.0f, 1.0f);
            setValue(min + t * (max - min));
        }

        void InputSlider::nudge(int inSteps)
        {
            float amount = step;
            if (amount <= 0.0f)
            {
                amount = std::max(0.01f, (max - min) * 0.01f);
            }

            setValue(value + amount * static_cast<float>(inSteps));
        }

        void InputSlider::commit()
        {
            const String raw = getAttribute(VALUE_ATTRIBUTE_NAME);
            if (!isReference(raw) || !hasParent())
            {
                return;
            }

            const String id = raw.getBetween(REFERENCE_VALUE_OPENING, REFERENCE_VALUE_CLOSING).trim();
            if (id.isEmpty())
            {
                return;
            }

            for (Component* node = getParent(); node != nullptr; node = node->getParent())
            {
                const ReflectionFieldAccessor accessor = node->getField(id);
                if (accessor.isValid() && accessor.isType<float>())
                {
                    void* instance = accessor.boundInstance != nullptr ? const_cast<void*>(accessor.boundInstance)
                                                                       : static_cast<void*>(node);

                    if (float* target = accessor.getValue<float>(instance))
                    {
                        *target = value;
                    }

                    return;
                }

                if (node->isRoot())
                {
                    return;
                }
            }
        }

        void InputSlider::emitInput()
        {
            getMethod(getAttribute(ON_INPUT_ATTRIBUTE_NAME)).invoke();
        }

        float InputSlider::parseNumber(const String& inValue, float inFallback) const
        {
            const String value = parseText(inValue).trim();
            if (value.isEmpty())
            {
                return inFallback;
            }

            char*       end    = nullptr;
            const float parsed = static_cast<float>(std::strtod(value.toChar(), &end));
            if (!end || end == value.toChar())
            {
                return inFallback;
            }

            return parsed;
        }

        float InputSlider::clampValue(float inValue) const
        {
            return std::clamp(inValue, min, max);
        }

        float InputSlider::snapValue(float inValue) const
        {
            if (step <= 0.0f)
            {
                return inValue;
            }

            const float origin  = inValue - min;
            const float snapped = std::round(origin / step) * step;

            return min + snapped;
        }

        void InputSlider::refreshPercentage()
        {
            const float span = max - min;
            if (span <= 0.0001f)
            {
                percentage = 0.0f;

                return;
            }

            percentage = std::clamp(((value - min) / span) * 100.0f, 0.0f, 100.0f);
        }
    }
}
