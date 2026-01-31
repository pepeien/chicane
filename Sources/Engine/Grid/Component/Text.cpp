#include "Chicane/Grid/Component/Text.hpp"

#include "Chicane/Grid/Component/Text/Character.hpp"

namespace Chicane
{
    namespace Grid
    {
        Text::Text(const pugi::xml_node& inNode)
            : Container(inNode),
              m_parsedText("")
        {
            setText(inNode.text().as_string());

            setStyle({
                {Style::DISPLAY_ATTRIBUTE_NAME,        Style::DISPLAY_TYPE_FLEX      },
                {Style::FLEX_DIRECTION_ATTRIBUTE_NAME, Style::FLEX_DIRECTION_TYPE_ROW},
                {Style::WIDTH_ATTRIBUTE_NAME,          Style::AUTO_SIZE_UNIT         }
            });
        }

        void Text::onRefresh()
        {
            Container::onRefresh();

            refreshText();

            m_style.height = m_style.font.size;
            m_style.gap.setAll(m_style.letterSpacing);
        }

        const String& Text::getText() const
        {
            return m_text;
        }

        void Text::setText(const String& inValue)
        {
            if (inValue.equals(m_text))
            {
                return;
            }

            m_text = inValue;
        }

        void Text::refreshText()
        {
            const String value = parseText(m_text);

            if (value.equals(m_parsedText))
            {
                return;
            }

            m_parsedText = value;

            for (std::uint32_t i = 0; i < m_parsedText.size(); i++)
            {
                if (i >= m_children.size())
                {
                    addChild(new TextCharacter());
                }

                Component* child = m_children.at(i);

                if (typeid(*child) != typeid(TextCharacter))
                {
                    continue;
                }

                static_cast<TextCharacter*>(child)->setCharacter(m_parsedText.at(i));
            }

            for (std::uint32_t i = m_parsedText.size(); i < m_children.size(); i++)
            {
                Component* child = m_children.at(i);

                if (typeid(*child) != typeid(TextCharacter))
                {
                    continue;
                }

                static_cast<TextCharacter*>(child)->disable();
            }
        }
    }
}