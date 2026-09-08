#include "Chicane/Grid/Component/Button/Check.reflected.hpp"

#include "Chicane/Core/Input/Keyboard/Button.hpp"
#include "Chicane/Core/Input/Keyboard/Event.hpp"

namespace Chicane
{
    namespace Grid
    {
        ButtonCheck::ButtonCheck(const pugi::xml_node& inNode)
            : Container(inNode),
              isChecked(false),
              checkState(STATE_IDLE),
              m_bIsEdited(false)
        {
            load("Assets/Engine/UI/Components/Button/Check.grid", "Assets/Engine/UI/Components/Button/Check.decal");
        }

        bool ButtonCheck::isFocusable() const
        {
            return true;
        }

        bool ButtonCheck::onEvent(const WindowEvent& inEvent)
        {
            if (!isFocused() || inEvent.type != WindowEventType::KeyDown || !inEvent.data)
            {
                return false;
            }

            const Input::KeyboardEvent event = *static_cast<Input::KeyboardEvent*>(inEvent.data);
            if (event.button != Input::KeyboardButton::Space && event.button != Input::KeyboardButton::Return)
            {
                return false;
            }

            toggle();

            return true;
        }

        void ButtonCheck::onTick(float inDeltaTime)
        {
            Container::onTick(inDeltaTime);

            refreshChecked();
        }

        void ButtonCheck::onClick()
        {
            toggle();
        }

        void ButtonCheck::toggle()
        {
            setChecked(!isChecked);
        }

        void ButtonCheck::check()
        {
            setChecked(true);
        }

        void ButtonCheck::uncheck()
        {
            setChecked(false);
        }

        void ButtonCheck::setChecked(bool inValue)
        {
            if (isChecked == inValue)
            {
                refreshState();

                return;
            }

            isChecked   = inValue;
            m_bIsEdited = true;
            refreshState();
            commit();
            emitInput();
        }

        void ButtonCheck::refreshChecked()
        {
            const String raw = getAttribute(CHECKED_ATTRIBUTE_NAME);
            if (raw.isEmpty())
            {
                refreshState();

                return;
            }

            if (!isReference(raw) && m_bIsEdited)
            {
                refreshState();

                return;
            }

            isChecked = parseChecked(raw, isChecked);
            refreshState();
        }

        void ButtonCheck::refreshState()
        {
            checkState = isChecked ? STATE_CHECKED : STATE_IDLE;
        }

        void ButtonCheck::commit()
        {
            const String raw = getAttribute(CHECKED_ATTRIBUTE_NAME);
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
                if (accessor.isValid() && accessor.isType<bool>())
                {
                    void* instance = accessor.boundInstance != nullptr ? const_cast<void*>(accessor.boundInstance)
                                                                       : static_cast<void*>(node);

                    if (bool* target = accessor.getValue<bool>(instance))
                    {
                        *target = isChecked;
                    }

                    return;
                }

                if (node->isRoot())
                {
                    return;
                }
            }
        }

        void ButtonCheck::emitInput()
        {
            getMethod(getAttribute(ON_INPUT_ATTRIBUTE_NAME)).invoke();
        }

        bool ButtonCheck::parseChecked(const String& inValue, bool inFallback) const
        {
            const String value = parseText(inValue).trim().toLower();
            if (value.isEmpty())
            {
                return inFallback;
            }

            if (value.equals("true", "1", "yes", "checked"))
            {
                return true;
            }

            if (value.equals("false", "0", "no"))
            {
                return false;
            }

            return inFallback;
        }
    }
}
