#include "Chicane/Grid/Component/Text.hpp"

#include "Chicane/Grid/Component/Text/Character.hpp"

namespace Chicane
{
    namespace Grid
    {
        Text::Text(const pugi::xml_node& inNode)
            : Container(inNode),
              m_text(""),
              m_font(nullptr)
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
            refreshFont();
            refreshText();

            Container::onRefresh();
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

        bool Text::hasFont() const
        {
            return m_font != nullptr;
        }

        void Text::refreshFont()
        {
            m_font = Box::getById<Box::Font>(m_style.font.family.get().toStandard());
        }

        void Text::refreshText()
        {
            if (!hasFont())
            {
                return;
            }

            const String value = parseText(m_text);

            if (value.equals(m_parsedText))
            {
                return;
            }

            const Box::FontParsed& fontFamily = m_font->getData();

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

                char32_t character = codepoints.at(i);

                if (!fontFamily.hasGlyph(character))
                {
                    continue;
                }

                static_cast<TextCharacter*>(child)->setGlyph(&fontFamily.getGlyph(character));
            }

            for (std::uint32_t i = codepoints.size(); i < m_children.size(); i++)
            {
                if (typeid(*m_children.at(i)) != typeid(TextCharacter))
                {
                    continue;
                }

                static_cast<TextCharacter*>(m_children.at(i))->setGlyph(nullptr);
            }
        }
    }
}