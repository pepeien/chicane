#include "Chicane/Grid/Component/Text/Input.reflected.hpp"

#include <cstdlib>

#include "Chicane/Core/Input/Keyboard/Event.hpp"
#include "Chicane/Core/Input/Text/Event.hpp"
#include "Chicane/Core/Window.hpp"

namespace Chicane
{
    namespace Grid
    {
        namespace
        {
            bool hasModifier(Input::KeyboardButtonModifier inValue, Input::KeyboardButtonModifier inFlag)
            {
                return (static_cast<std::uint16_t>(inValue) & static_cast<std::uint16_t>(inFlag)) != 0;
            }

            String characterFromKey(const Input::KeyboardEvent& inEvent)
            {
                if (hasModifier(inEvent.modifier, Input::KeyboardButtonModifier::Ctrl) ||
                    hasModifier(inEvent.modifier, Input::KeyboardButtonModifier::Alt) ||
                    hasModifier(inEvent.modifier, Input::KeyboardButtonModifier::Gui))
                {
                    return String::empty();
                }

                if (inEvent.key < 32 || inEvent.key > 126)
                {
                    return String::empty();
                }

                char character = static_cast<char>(inEvent.key);
                if (character >= 'a' && character <= 'z')
                {
                    const bool bShift = hasModifier(inEvent.modifier, Input::KeyboardButtonModifier::Shift);
                    const bool bCaps  = hasModifier(inEvent.modifier, Input::KeyboardButtonModifier::Caps);
                    if (bShift != bCaps)
                    {
                        character = static_cast<char>(character - 32);
                    }
                }

                return String(character);
            }
        }

        TextInput::TextInput(const pugi::xml_node& inNode)
            : Text(inNode),
              value(String::empty()),
              caretX(0.0f),
              m_bEdited(false),
              m_bReplaceOnInput(false),
              m_pendingText(String::empty())
        {
            load("Assets/Engine/UI/Components/Text/Input.grid", "Assets/Engine/UI/Components/Text/Input.decal");
        }

        bool TextInput::isFocusable() const
        {
            return true;
        }

        bool TextInput::onEvent(const WindowEvent& inEvent)
        {
            if (inEvent.type == WindowEventType::WindowFocusLost)
            {
                setTextInputActive(false);

                return true;
            }

            if (inEvent.type == WindowEventType::WindowFocusGained)
            {
                if (isFocused())
                {
                    setTextInputActive(true);
                }

                return true;
            }

            if (Scrollable::onEvent(inEvent))
            {
                return true;
            }

            if (!isFocused())
            {
                return false;
            }

            if (inEvent.type == WindowEventType::TextInput)
            {
                if (!inEvent.data)
                {
                    return false;
                }

                const Input::TextEvent event = *static_cast<Input::TextEvent*>(inEvent.data);
                if (event.text.isEmpty() || event.text.equals(m_pendingText))
                {
                    m_pendingText = String::empty();

                    return true;
                }

                insert(event.text);
                m_pendingText = String::empty();

                return true;
            }

            if (inEvent.type != WindowEventType::KeyDown && inEvent.type != WindowEventType::KeyUp)
            {
                return false;
            }

            if (!inEvent.data)
            {
                return true;
            }

            const Input::KeyboardEvent event = *static_cast<Input::KeyboardEvent*>(inEvent.data);

            if (inEvent.type == WindowEventType::KeyDown)
            {
                if (event.button == Input::KeyboardButton::Backspace)
                {
                    erase();
                    m_pendingText = String::empty();

                    return true;
                }

                const String character = characterFromKey(event);
                if (character.isEmpty())
                {
                    return true;
                }

                insert(character);
                m_pendingText = character;

                return true;
            }

            return true;
        }

        void TextInput::onRefresh()
        {
            refreshValue();
            setText(value);

            Text::onRefresh();

            caretX = getContentSize().x;
        }

        void TextInput::onFocus()
        {
            m_bReplaceOnInput = true;
            setTextInputActive(true);
            refreshStyleSubtree();
        }

        void TextInput::onBlur()
        {
            m_pendingText = String::empty();
            setTextInputActive(false);
            refreshStyleSubtree();
        }

        void TextInput::refreshSize()
        {
            m_style.width.refresh();
            m_style.height.refresh();

            Component::refreshSize();
        }

        void TextInput::refreshValue()
        {
            const String raw = getAttribute(VALUE_ATTRIBUTE_NAME);
            if (raw.isEmpty() || isFocused())
            {
                return;
            }

            if (!isReference(raw) && m_bEdited)
            {
                return;
            }

            value = parseText(raw);
        }

        void TextInput::insert(const String& inText)
        {
            if (inText.isEmpty() || inText.equals("\n", "\r", "\t"))
            {
                return;
            }

            if (m_bReplaceOnInput)
            {
                value             = String::empty();
                m_bReplaceOnInput = false;
            }

            value.append(inText);
            m_bEdited = true;
            setText(value);

            commit();
            emitInput();
        }

        void TextInput::erase()
        {
            if (m_bReplaceOnInput)
            {
                value             = String::empty();
                m_bReplaceOnInput = false;
                m_bEdited         = true;
                setText(value);

                commit();
                emitInput();

                return;
            }

            if (value.isEmpty())
            {
                return;
            }

            do
            {
                const unsigned char character = static_cast<unsigned char>(value.back());
                value.popBack();

                if ((character & 0xC0) != 0x80)
                {
                    break;
                }
            } while (!value.isEmpty());

            m_bEdited = true;
            setText(value);

            commit();
            emitInput();
        }

        void TextInput::commit()
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
                if (accessor.isValid())
                {
                    void* instance = accessor.boundInstance != nullptr ? const_cast<void*>(accessor.boundInstance)
                                                                       : static_cast<void*>(node);

                    if (accessor.isType<String>())
                    {
                        if (String* target = accessor.getValue<String>(instance))
                        {
                            *target = value;
                        }

                        return;
                    }

                    if (accessor.isType<float>())
                    {
                        const String trimmed = value.trim();
                        if (trimmed.isEmpty())
                        {
                            return;
                        }

                        char*       end    = nullptr;
                        const float parsed = std::strtof(trimmed.toChar(), &end);
                        if (!end || end == trimmed.toChar() || *end != '\0')
                        {
                            return;
                        }

                        if (float* target = accessor.getValue<float>(instance))
                        {
                            *target = parsed;
                        }

                        return;
                    }
                }

                if (node->isRoot())
                {
                    return;
                }
            }
        }

        void TextInput::emitInput()
        {
            getMethod(getAttribute(ON_INPUT_ATTRIBUTE_NAME)).invoke();
        }

        void TextInput::setTextInputActive(bool inValue)
        {
            Window* window = Window::getCurrent();
            if (!window)
            {
                return;
            }

            if (inValue)
            {
                window->startTextInput();

                return;
            }

            window->stopTextInput();
        }
    }
}
