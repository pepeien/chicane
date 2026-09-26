#include "Chicane/Grid/Component/Input/Select/Option.reflected.hpp"

#include "Chicane/Grid/Component/Input/Select.hpp"
#include "Chicane/Grid/Component/Text.hpp"

namespace Chicane
{
    namespace Grid
    {
        InputSelectOption::InputSelectOption(const XmlNode& inNode)
            : Button(inNode),
              value(String::sEmpty())
        {
            ensureText(String(inNode.getText()).trim());
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

            XmlDocument document;
            XmlNode     node = document.appendChild(Text::TAG_ID);
            node.setText(inText);

            if (Component* child = sCreate(node))
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
