#include "Chicane/Grid/Component/Character.hpp"

namespace Chicane
{
    namespace Grid
    {
        static constexpr inline const char NULL_CHARACTER = '\0';

        static const Box::Font::Parsed EMPTY_FONT  = {};
        static const Box::Font::Glyph      EMPTY_GLYPH = {};

        Character::Character()
            : Super(TAG_ID),
            m_canUpdate(false),
            m_character(NULL_CHARACTER)
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
            return m_parent->isVisible() && hasGlyph() && hasPrimitive();
        }

        void Character::onRefresh()
        {
            refreshFont();
        }

        void Character::refreshPrimitive()
        {
            if (!m_canUpdate)
            {
                return;
            }

            if (!hasGlyph() || !Color::isVisible(m_style.foregroundColor))
            {
                clearPrimitive();
    
                return;
            }

            m_canUpdate = false;

            const Box::Font::Glyph& glyph = getGlyph();

            Primitive primitive = {};
            primitive.indices = glyph.indices;

            Vertex vertex = {};
            vertex.color  = m_style.foregroundColor;

            for (const Vec3 position : glyph.vertices)
            {
                vertex.position = position;
    
                primitive.vertices.push_back(vertex);
            }

            setPrimitive(primitive);
        }

        void Character::disable()
        {
            setCharacter(NULL_CHARACTER);
        }

        bool Character::hasCharacter() const
        {
            return m_character != NULL_CHARACTER;
        }

        char Character::getCharacter() const
        {
            return m_character;
        }

        void Character::setCharacter(char inValue)
        {
            if (m_character == inValue)
            {
                return;
            }

            setProperty(m_character, inValue);
        }

        bool Character::hasFont() const
        {
            return Box::getFontManager()->isFamilyAllocated(m_style.font.family);
        }

        const Box::Font::Parsed& Character::getFont() const
        {
            if (!hasFont())
            {
                return EMPTY_FONT;
            }

            return Box::getFontManager()->getByFamily(m_style.font.family);
        }

        bool Character::hasGlyph() const
        {
            return hasCharacter() && hasFont() && getFont().hasGlyph(m_character);
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

            m_style.width         = m_style.font.size;
            m_style.height        = m_style.font.size;
            m_style.margin.right  = glyph.bearing.x;
            //m_style.margin.bottom = glyph.bearing.y;

            m_canUpdate = true;

            refreshPrimitive();
        }
    }
}