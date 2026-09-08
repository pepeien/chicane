#include "Chicane/Grid/Component/Button/Radio.reflected.hpp"

#include "Chicane/Core/Input/Keyboard/Button.hpp"
#include "Chicane/Core/Input/Keyboard/Event.hpp"

namespace Chicane
{
    namespace Grid
    {
        ButtonRadio::ButtonRadio(const pugi::xml_node& inNode)
            : Container(inNode),
              isChecked(false),
              group(String::empty()),
              value(String::empty()),
              checkState(STATE_IDLE),
              m_bIsEdited(false)
        {
            load("Assets/Engine/UI/Components/Button/Radio.grid", "Assets/Engine/UI/Components/Button/Radio.decal");
        }

        bool ButtonRadio::isFocusable() const
        {
            return true;
        }

        bool ButtonRadio::onEvent(const WindowEvent& inEvent)
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

            select();

            return true;
        }

        void ButtonRadio::onTick(float inDeltaTime)
        {
            Container::onTick(inDeltaTime);

            refreshAttributes();
        }

        void ButtonRadio::onClick()
        {
            select();
        }

        void ButtonRadio::select()
        {
            if (isChecked)
            {
                refreshState();

                return;
            }

            clearGroup();
            setChecked(true, true);
        }

        void ButtonRadio::setChecked(bool inValue, bool bShouldEmit)
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

            if (bShouldEmit)
            {
                emitInput();
            }
        }

        void ButtonRadio::refreshAttributes()
        {
            group = parseText(getAttribute(GROUP_ATTRIBUTE_NAME)).trim();
            value = parseText(getAttribute(VALUE_ATTRIBUTE_NAME)).trim();

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

        void ButtonRadio::refreshState()
        {
            checkState = isChecked ? STATE_CHECKED : STATE_IDLE;
        }

        void ButtonRadio::clearGroup()
        {
            if (group.isEmpty() || !hasRoot())
            {
                return;
            }

            clearGroupFrom(getRoot());
        }

        void ButtonRadio::clearGroupFrom(Component* inNode)
        {
            if (!inNode)
            {
                return;
            }

            for (Component* child : inNode->getChildren())
            {
                if (!child)
                {
                    continue;
                }

                if (child != this && child->getTag().equals(TAG_ID))
                {
                    ButtonRadio* radio = static_cast<ButtonRadio*>(child);
                    if (radio->group.equals(group))
                    {
                        radio->setChecked(false, false);
                    }
                }

                clearGroupFrom(child);
            }
        }

        void ButtonRadio::commit()
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

        void ButtonRadio::emitInput()
        {
            getMethod(getAttribute(ON_INPUT_ATTRIBUTE_NAME)).invoke();
        }

        bool ButtonRadio::parseChecked(const String& inValue, bool inFallback) const
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
