#include "Chicane/Grid/Component/Input/Select/Option.reflected.hpp"

#include "Chicane/Grid/Component/Input/Select.hpp"
#include "Chicane/Grid/Component/Text.hpp"

namespace Chicane
{
    namespace Grid
    {
        InputSelectOption::InputSelectOption(const pugi::xml_node& inNode)
            : Button(inNode),
              value(String::empty())
        {
            ensureText(String(inNode.text().as_string()).trim());
            refreshValue();
        }

        String InputSelectOption::getLabel() const
        {
            for (Component* child : getChildren())
            {
                if (!child || !child->getTag().equals(Text::TAG_ID))
                {
                    continue;
                }

                const String parsed = child->parseText(static_cast<Text*>(child)->getText()).trim();
                if (!parsed.isEmpty())
                {
                    return parsed;
                }
            }

            return value;
        }

        void InputSelectOption::onRefresh()
        {
            Button::onRefresh();

            refreshValue();
            refreshClass();
        }

        void InputSelectOption::onClick()
        {
            if (InputSelect* InputSelect = findInputSelect())
            {
                InputSelect->pick(this);
            }
        }

        void InputSelectOption::ensureText(const String& inText)
        {
            if (inText.isEmpty())
            {
                return;
            }

            for (Component* child : getChildren())
            {
                if (child && child->getTag().equals(Text::TAG_ID))
                {
                    return;
                }
            }

            pugi::xml_document document;
            pugi::xml_node     node = document.append_child(Text::TAG_ID);
            node.append_child(pugi::node_pcdata).set_value(inText.toChar());

            if (Component* child = create(node))
            {
                addChild(child);
            }
        }

        void InputSelectOption::refreshValue()
        {
            const String raw = getAttribute(VALUE_ATTRIBUTE_NAME);
            if (!raw.isEmpty())
            {
                value = parseText(raw).trim();

                return;
            }

            const String text = getLabel();
            if (!text.isEmpty())
            {
                value = text;
            }
        }

        void InputSelectOption::refreshClass()
        {
            String className = parseText(getAttribute(CLASS_ATTRIBUTE_NAME)).trim();

            const InputSelect* InputSelect = findInputSelect();
            if (InputSelect)
            {
                const String optionValue = value.isEmpty() ? getLabel() : value;
                if (!optionValue.isEmpty() && optionValue.equals(InputSelect->value))
                {
                    if (!className.isEmpty())
                    {
                        className.append(' ');
                    }

                    className.append("--InputSelected");
                }

                if (InputSelect->isHighlighted(this))
                {
                    if (!className.isEmpty())
                    {
                        className.append(' ');
                    }

                    className.append("--active");
                }
            }

            if (!className.equals(getClassName()))
            {
                setClassName(className);
            }
        }

        InputSelect* InputSelectOption::findInputSelect() const
        {
            Component* node = m_parent;
            while (node)
            {
                if (node->getTag().equals(InputSelect::TAG_ID))
                {
                    return static_cast<InputSelect*>(node);
                }

                if (node->isRoot())
                {
                    break;
                }

                node = node->getParent();
            }

            return nullptr;
        }
    }
}
