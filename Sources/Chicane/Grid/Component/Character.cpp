#include "Chicane/Grid/Component/Character.hpp"

namespace Chicane
{
    namespace Grid
    {
        static const Box::Font::ParsedData EMPTY_FONT  = {};
        static const Box::Font::Glyph      EMPTY_GLYPH = {};

        Character::Character()
            : Component(TAG_ID),
            m_character('\0')
        {
            Box::getFontManager()->watchChanges(
                [this](Box::Manager::EventType event)
                {
                    refreshFont();
                }
            );
        }

        bool Character::isDrawable() const
        {
            return m_parent->isVisible() && hasGlyph() && !isPrimitiveEmpty();
        }

        void Character::refreshPrimitive()
        {
            refreshFont();

            m_primitive.clear();

            if (!hasGlyph() || !Color::isVisible(m_style.foregroundColor))
            {
                return;
            }

            const Box::Font::Glyph& glyph = getGlyph();

            Vertex vertex = {};
            vertex.color  = m_style.foregroundColor;

            for (std::uint32_t index : glyph.indices)
            {
                vertex.position = glyph.vertices.at(index);
    
                m_primitive.push_back(vertex);
            }
        }

        char Character::getCharacter() const
        {
            return m_character;
        }

        void Character::setCharacter(char inValue)
        {
            if (m_character == inValue || inValue == '\0')
            {
                return;
            }

            m_character = inValue;

            refreshFont();
        }

        bool Character::hasFont() const
        {
            return Box::getFontManager()->isFamilyAllocated(m_style.font.family);
        }

        const Box::Font::ParsedData& Character::getFont() const
        {
            if (!hasFont())
            {
                return EMPTY_FONT;
            }

            return Box::getFontManager()->getByFamily(m_style.font.family);
        }

        bool Character::hasGlyph() const
        {
            return hasFont() && getFont().hasGlyph(m_character);
        }

        const Box::Font::Glyph& Character::getGlyph() const
        {
            if (!hasGlyph())
            {
                return EMPTY_GLYPH;
            }

            return getFont().getGlyph(m_character);
        }

        void Character::refreshFont()
        {
            if (!hasParent())
            {
                return;
            }

            const Style::Instance& parentStyle = m_parent->getStyle();

            m_style.foregroundColor = parentStyle.foregroundColor;
            m_style.font            = parentStyle.font;

            if (!hasGlyph() || !Color::isVisible(m_style.foregroundColor))
            {
                return;
            }

            const Box::Font::Glyph& glyph = getGlyph();

            m_style.width           = m_style.font.size;
            m_style.height          = m_style.font.size;
            //m_style.margin.right    = glyph.bearing.x;
            //m_style.margin.bottom   = glyph.bearing.y;
        }
    }
}