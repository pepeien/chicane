#include "Chicane/Grid/Component/Text.hpp"

#include "Chicane/Grid/Component/Character.hpp"

namespace Chicane
{
    namespace Grid
    {
        Text::Text(const pugi::xml_node& inNode)
            : Component(inNode),
            m_lastText("")
        {
            setText(inNode.text().as_string());
        }

        void Text::onTick(float inDelta)
        {
            refreshText();
        }

        const std::string& Text::getText() const
        {
            return m_text;
        }

        void Text::setText(const std::string& inValue)
        {
            if (inValue.empty())
            {
                return;
            }

            if (String::areEquals(inValue, m_text))
            {
                return;
            }

            m_text = inValue;

            refreshText();
        }

        void Text::refreshText()
        {
            const std::string value = parseText(m_text);

            if (String::areEquals(value, m_lastText))
            {
                return;
            }

            m_lastText = value;

            for (std::uint32_t i = 0; i < m_lastText.size(); i++)
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
                    m_lastText.at(i)
                );
            }

            for (std::uint32_t i = m_lastText.size(); i < m_children.size(); i++)
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