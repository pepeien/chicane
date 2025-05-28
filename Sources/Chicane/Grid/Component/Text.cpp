#include "Chicane/Grid/Component/Text.hpp"

#include "Chicane/Grid/Component/Character.hpp"

namespace Chicane
{
    namespace Grid
    {
        Text::Text(const pugi::xml_node& inNode)
            : Super(inNode),
            m_parsedText("")
        {
            setText(inNode.text().as_string());
        }

        void Text::onRefresh()
        {
            refreshText();
        }

        const std::string& Text::getText() const
        {
            return m_text;
        }

        void Text::setText(const std::string& inValue)
        {
            if (String::areEquals(inValue, m_text))
            {
                return;
            }

            m_text = inValue;
        }

        void Text::refreshText()
        {
            const std::string value = parseText(m_text);

            if (String::areEquals(value, m_parsedText))
            {
                return;
            }

            m_parsedText = value;

            for (std::uint32_t i = 0; i < m_parsedText.size(); i++)
            {
                if (i >= m_children.size())
                {
                    addChild(new Character());
                }

                Component* child = m_children.at(i);

                if (typeid(*child) != typeid(Character))
                {
                    continue;
                }

                static_cast<Character*>(child)->setCharacter(
                    m_parsedText.at(i)
                );
            }

            for (std::uint32_t i = m_parsedText.size(); i < m_children.size(); i++)
            {
                Component* child = m_children.at(i);

                if (typeid(*child) != typeid(Character))
                {
                    continue;
                }

                static_cast<Character*>(child)->disable();
            }
        }
    }
}