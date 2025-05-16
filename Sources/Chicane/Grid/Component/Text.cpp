#include "Chicane/Grid/Component/Text.hpp"

#include "Chicane/Grid/Component/Character.hpp"

namespace Chicane
{
    namespace Grid
    {
        Text::Text(const pugi::xml_node& inNode)
            : Component(inNode)
        {
            setText(inNode.text().as_string());
        }

        void Text::onTick(float inDelta)
        {
            setText(parseText(m_text));
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

            for (std::uint32_t i = 0; i < m_text.size(); i++)
            {
                char character = m_text.at(i);

                if (i >= m_children.size())
                {
                    addChild(new Character());
                }

                if (typeid(*m_children.at(i)) != typeid(Character))
                {
                    continue;
                }

                static_cast<Character*>(m_children.at(i))->setCharacter(character);
            }
        }
    }
}