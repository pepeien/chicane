#include "Chicane/Grid/Component/Input/Select.reflected.hpp"

#include "Chicane/Core/Input/Keyboard/Event.hpp"

#include "Chicane/Grid/Component/Input/Select/Option.hpp"
#include "Chicane/Grid/Component/Scrollable.hpp"

namespace Chicane
{
    namespace Grid
    {
        constexpr std::size_t INVALID_HIGHLIGHT = static_cast<std::size_t>(-1);

        InputSelect::InputSelect(const pugi::xml_node& inNode)
            : Container(inNode),
              value(String::empty()),
              label(String::empty()),
              placeholder(String::empty()),
              isOpen(false),
              items({}),
              m_bIsEdited(false),
              m_highlighted(INVALID_HIGHLIGHT)
        {
            load("Assets/Engine/UI/Components/Input/Select.grid", "Assets/Engine/UI/Components/Input/Select.decal");
        }

        bool InputSelect::isFocusable() const
        {
            return true;
        }

        bool InputSelect::onEvent(const WindowEvent& inEvent)
        {
            if (Scrollable::onEvent(inEvent))
            {
                return true;
            }

            if (!isFocused() || inEvent.type != WindowEventType::KeyDown || !inEvent.data)
            {
                return false;
            }

            const Input::KeyboardEvent event = *static_cast<const Input::KeyboardEvent*>(inEvent.data);

            if (event.button == Input::KeyboardButton::Escape)
            {
                if (!isOpen)
                {
                    return false;
                }

                close();

                return true;
            }

            if (event.button == Input::KeyboardButton::Return || event.button == Input::KeyboardButton::Space)
            {
                if (!isOpen)
                {
                    open();

                    return true;
                }

                chooseHighlighted();

                return true;
            }

            if (event.button == Input::KeyboardButton::Down)
            {
                if (!isOpen)
                {
                    open();

                    return true;
                }

                moveHighlight(1);

                return true;
            }

            if (event.button == Input::KeyboardButton::Up)
            {
                if (!isOpen)
                {
                    open();

                    return true;
                }

                moveHighlight(-1);

                return true;
            }

            return false;
        }

        std::vector<InputSelectOption*> InputSelect::getOptions() const
        {
            std::vector<InputSelectOption*> result;

            for (Component* child : getChildrenFlat())
            {
                if (!child || !child->getTag().equals(InputSelectOption::TAG_ID))
                {
                    continue;
                }

                result.push_back(static_cast<InputSelectOption*>(child));
            }

            return result;
        }

        std::vector<InputSelectOption*> InputSelect::getVisibleOptions() const
        {
            std::vector<InputSelectOption*> result;

            for (InputSelectOption* option : getOptions())
            {
                if (!option || !option->isDisplayable())
                {
                    continue;
                }

                result.push_back(option);
            }

            return result;
        }

        bool InputSelect::isHighlighted(const InputSelectOption* inOption) const
        {
            if (!inOption || m_highlighted == INVALID_HIGHLIGHT)
            {
                return false;
            }

            const std::vector<InputSelectOption*> options = getVisibleOptions();
            if (m_highlighted >= options.size())
            {
                return false;
            }

            return options.at(m_highlighted) == inOption;
        }

        void InputSelect::tick(float inDeltaTime)
        {
            Scrollable::tick(inDeltaTime);

            refreshLabel();
            refreshHighlight();
        }

        void InputSelect::onTick(float inDeltaTime)
        {
            refreshItems();
            refreshValue();
        }

        void InputSelect::onFocus()
        {
            refreshStyleSubtree();
        }

        void InputSelect::onBlur()
        {
            close();

            refreshStyleSubtree();
        }

        void InputSelect::toggle()
        {
            if (isOpen)
            {
                close();

                return;
            }

            open();
        }

        void InputSelect::open()
        {
            if (isOpen)
            {
                return;
            }

            isOpen = true;
            refreshHighlight();
            refreshStyleSubtree();
        }

        void InputSelect::close()
        {
            if (!isOpen)
            {
                return;
            }

            isOpen        = false;
            m_highlighted = INVALID_HIGHLIGHT;

            refreshStyleSubtree();
        }

        void InputSelect::choose(String inValue)
        {
            value       = inValue;
            m_bIsEdited = true;

            refreshLabel();
            commit();
            emitInput();
            close();
        }

        void InputSelect::pick(InputSelectOption* inOption)
        {
            if (!inOption)
            {
                return;
            }

            const String next = inOption->value.isEmpty() ? inOption->getLabel() : inOption->value;
            choose(next);

            if (label.isEmpty())
            {
                label = inOption->getLabel();
            }
        }

        void InputSelect::refreshValue()
        {
            const String raw = getAttribute(VALUE_ATTRIBUTE_NAME);
            if (raw.isEmpty() || isOpen)
            {
                return;
            }

            if (!isReference(raw) && m_bIsEdited)
            {
                return;
            }

            value = parseText(raw).trim();
        }

        void InputSelect::refreshItems()
        {
            const String raw = getAttribute(ITEMS_ATTRIBUTE_NAME);
            if (raw.isEmpty())
            {
                return;
            }

            if (!isReference(raw))
            {
                items.clear();

                for (const String& token : raw.split(','))
                {
                    const String item = parseText(token).trim();
                    if (item.isEmpty())
                    {
                        continue;
                    }

                    items.push_back(item);
                }

                return;
            }

            const String id = raw.getBetween(REFERENCE_VALUE_OPENING, REFERENCE_VALUE_CLOSING).trim();
            if (id.isEmpty())
            {
                return;
            }

            for (Component* node = this; node != nullptr; node = node->getParent())
            {
                const ReflectionFieldAccessor accessor = node->getField(id);
                if (accessor.isValid() && accessor.isType<std::vector<String>>())
                {
                    void* instance = accessor.boundInstance != nullptr ? const_cast<void*>(accessor.boundInstance)
                                                                       : static_cast<void*>(node);

                    if (const std::vector<String>* list = accessor.getValue<std::vector<String>>(instance))
                    {
                        items = *list;
                    }

                    return;
                }

                if (node->isRoot())
                {
                    return;
                }
            }
        }

        void InputSelect::refreshLabel()
        {
            const String raw = getAttribute(PLACEHOLDER_ATTRIBUTE_NAME);
            placeholder      = raw.isEmpty() ? String::empty() : parseText(raw).trim();

            for (InputSelectOption* option : getOptions())
            {
                if (!option)
                {
                    continue;
                }

                const String optionValue = option->value.isEmpty() ? option->getLabel() : option->value;
                if (optionValue.equals(value))
                {
                    const String optionLabel = option->getLabel();
                    label                    = optionLabel.isEmpty() ? optionValue : optionLabel;

                    return;
                }
            }

            if (!value.isEmpty())
            {
                label = value;

                return;
            }

            label = placeholder;
        }

        void InputSelect::refreshHighlight()
        {
            if (!isOpen)
            {
                m_highlighted = INVALID_HIGHLIGHT;

                return;
            }

            const std::vector<InputSelectOption*> options = getVisibleOptions();
            if (options.empty())
            {
                m_highlighted = INVALID_HIGHLIGHT;

                return;
            }

            if (m_highlighted < options.size())
            {
                return;
            }

            for (std::size_t i = 0; i < options.size(); ++i)
            {
                InputSelectOption* option = options.at(i);
                if (!option)
                {
                    continue;
                }

                const String optionValue = option->value.isEmpty() ? option->getLabel() : option->value;
                if (optionValue.equals(value))
                {
                    m_highlighted = i;

                    return;
                }
            }

            m_highlighted = 0;
        }

        void InputSelect::commit()
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
                if (accessor.isValid() && accessor.isType<String>())
                {
                    void* instance = accessor.boundInstance != nullptr ? const_cast<void*>(accessor.boundInstance)
                                                                       : static_cast<void*>(node);

                    if (String* target = accessor.getValue<String>(instance))
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

        void InputSelect::emitInput()
        {
            getMethod(getAttribute(ON_INPUT_ATTRIBUTE_NAME)).invoke();
        }

        void InputSelect::moveHighlight(int inDelta)
        {
            const std::vector<InputSelectOption*> options = getVisibleOptions();
            if (options.empty())
            {
                m_highlighted = INVALID_HIGHLIGHT;

                return;
            }

            const int count = static_cast<int>(options.size());
            int       next  = m_highlighted == INVALID_HIGHLIGHT ? 0 : static_cast<int>(m_highlighted) + inDelta;

            while (next < 0)
            {
                next += count;
            }

            m_highlighted = static_cast<std::size_t>(next % count);
        }

        void InputSelect::chooseHighlighted()
        {
            const std::vector<InputSelectOption*> options = getVisibleOptions();
            if (m_highlighted >= options.size())
            {
                close();

                return;
            }

            pick(options.at(m_highlighted));
        }
    }
}
