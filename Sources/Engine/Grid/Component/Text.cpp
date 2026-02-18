#include "Chicane/Grid/Component/Text.hpp"

#include "Chicane/Grid/Component/Text/Character.hpp"

namespace Chicane
{
    namespace Grid
    {
        Text::Text(const pugi::xml_node& inNode)
            : Component(inNode),
              m_text("")
        {
            setText(inNode.text().as_string());

            addStyleProperties({
                {Style::DISPLAY_ATTRIBUTE_NAME,        Style::DISPLAY_TYPE_FLEX      },
                {Style::FLEX_DIRECTION_ATTRIBUTE_NAME, Style::FLEX_DIRECTION_TYPE_ROW},
                {Style::FONT_SIZE_ATTRIBUTE_NAME,      Style::FONT_SIZE_DEFAULT_VALUE}
            });
        }

        void Text::onRefresh()
        {
            refreshText();
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

            std::vector<char32_t> codepoints = value.toUnicode();

            for (std::uint32_t i = 0; i < codepoints.size(); i++)
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

                static_cast<TextCharacter*>(child)->setCharacter(codepoints.at(i));
            }

            for (std::uint32_t i = codepoints.size(); i < m_children.size(); i++)
            {
                if (typeid(*m_children.at(i)) != typeid(TextCharacter))
                {
                    continue;
                }

                static_cast<TextCharacter*>(m_children.at(i))->disable();
            }
        }
    }
}