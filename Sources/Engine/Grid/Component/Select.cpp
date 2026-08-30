#include "Chicane/Grid/Component/Select.reflected.hpp"

#include "Chicane/Core/Input/Keyboard/Event.hpp"

#include "Chicane/Grid/Component/Scrollable.hpp"
#include "Chicane/Grid/Component/Select/Option.hpp"

namespace Chicane
{
    namespace Grid
    {
        constexpr std::size_t kInvalidHighlight = static_cast<std::size_t>(-1);

        Select::Select(const pugi::xml_node& inNode)
            : Container(inNode),
              value(String::empty()),
              label(String::empty()),
              placeholder(String::empty()),
              isOpen(false),
              items({}),
              m_bEdited(false),
              m_highlighted(kInvalidHighlight)
        {
            load("Assets/Engine/UI/Components/Select.grid", "Assets/Engine/UI/Components/Select.decal");
        }

        bool Select::isFocusable() const
        {
            return true;
        }

        bool Select::onEvent(const WindowEvent& inEvent)
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

        std::vector<SelectOption*> Select::getOptions() const
        {
            std::vector<SelectOption*> result;

            for (Component* child : getChildrenFlat())
            {
                if (!child || !child->getTag().equals(SelectOption::TAG_ID))
                {
                    continue;
                }

                result.push_back(static_cast<SelectOption*>(child));
            }

            return result;
        }

        std::vector<SelectOption*> Select::getVisibleOptions() const
        {
            std::vector<SelectOption*> result;

            for (SelectOption* option : getOptions())
            {
                if (!option || !option->isDisplayable())
                {
                    continue;
                }

                result.push_back(option);
            }

            return result;
        }

        bool Select::isHighlighted(const SelectOption* inOption) const
        {
            if (!inOption || m_highlighted == kInvalidHighlight)
            {
                return false;
            }

            const std::vector<SelectOption*> options = getVisibleOptions();
            if (m_highlighted >= options.size())
            {
                return false;
            }

            return options.at(m_highlighted) == inOption;
        }

        void Select::tick(float inDeltaTime)
        {
            Scrollable::tick(inDeltaTime);

            refreshLabel();
            refreshHighlight();
        }

        void Select::onTick(float inDeltaTime)
        {
            refreshItems();
            refreshValue();
        }

        void Select::onFocus()
        {
            refreshStyleSubtree();
        }

        void Select::onBlur()
        {
            close();

            refreshStyleSubtree();
        }

        void Select::toggle()
        {
            if (isOpen)
            {
                close();

                return;
            }

            open();
        }

        void Select::open()
        {
            if (isOpen)
            {
                return;
            }

            isOpen = true;
            refreshHighlight();
            refreshStyleSubtree();
        }

        void Select::close()
        {
            if (!isOpen)
            {
                return;
            }

            isOpen        = false;
            m_highlighted = kInvalidHighlight;

            refreshStyleSubtree();
        }

        void Select::choose(String inValue)
        {
            value     = inValue;
            m_bEdited = true;

            refreshLabel();
            commit();
            emitInput();
            close();
        }

        void Select::pick(SelectOption* inOption)
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

        void Select::refreshValue()
        {
            const String raw = getAttribute(VALUE_ATTRIBUTE_NAME);
            if (raw.isEmpty() || isOpen)
            {
                return;
            }

            if (!isReference(raw) && m_bEdited)
            {
                return;
            }

            value = parseText(raw).trim();
        }

        void Select::refreshItems()
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

        void Select::refreshLabel()
        {
            const String raw = getAttribute(PLACEHOLDER_ATTRIBUTE_NAME);
            placeholder      = raw.isEmpty() ? String::empty() : parseText(raw).trim();

            for (SelectOption* option : getOptions())
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

        void Select::refreshHighlight()
        {
            if (!isOpen)
            {
                m_highlighted = kInvalidHighlight;

                return;
            }

            const std::vector<SelectOption*> options = getVisibleOptions();
            if (options.empty())
            {
                m_highlighted = kInvalidHighlight;

                return;
            }

            if (m_highlighted < options.size())
            {
                return;
            }

            for (std::size_t i = 0; i < options.size(); ++i)
            {
                SelectOption* option = options.at(i);
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

        void Select::commit()
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

        void Select::emitInput()
        {
            getMethod(getAttribute(ON_INPUT_ATTRIBUTE_NAME)).invoke();
        }

        void Select::moveHighlight(int inDelta)
        {
            const std::vector<SelectOption*> options = getVisibleOptions();
            if (options.empty())
            {
                m_highlighted = kInvalidHighlight;

                return;
            }

            const int count = static_cast<int>(options.size());
            int       next  = m_highlighted == kInvalidHighlight ? 0 : static_cast<int>(m_highlighted) + inDelta;

            while (next < 0)
            {
                next += count;
            }

            m_highlighted = static_cast<std::size_t>(next % count);
        }

        void Select::chooseHighlighted()
        {
            const std::vector<SelectOption*> options = getVisibleOptions();
            if (m_highlighted >= options.size())
            {
                close();

                return;
            }

            pick(options.at(m_highlighted));
        }
    }
}
