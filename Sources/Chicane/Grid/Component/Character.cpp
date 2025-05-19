#include "Chicane/Grid/Component/Character.hpp"

namespace Chicane
{
    namespace Grid
    {
        static constexpr const char NULL_CHARACTER = '\0';

        static const Box::Font::ParsedData EMPTY_FONT  = {};
        static const Box::Font::Glyph      EMPTY_GLYPH = {};

        Character::Character()
            : Component(TAG_ID),
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

        void Character::refreshPrimitive()
        {
            refreshFont();

            if (!m_canUpdate)
            {
                return;
            }

            m_primitive.clear();

            if (!hasGlyph() || !Color::isVisible(m_style.foregroundColor))
            {
                return;
            }

            const Box::Font::Glyph& glyph = getGlyph();

            Vertex vertex = {};
            vertex.color  = m_style.foregroundColor;

            for (const Vec<3, float> position : glyph.vertices)
            {
                vertex.position = position;
    
                m_primitive.vertices.push_back(vertex);
            }

            m_primitive.indices = glyph.indices;

            m_canUpdate = false;

            emmitChangesToParent();
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

            m_character = inValue;

            m_canUpdate = true;

            if (!hasCharacter())
            {
                return;
            }

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
            m_style.margin.bottom = glyph.bearing.y;
        }
    }
}