#include "Chicane/Grid/Component/Character.hpp"

#include "Chicane/Box/Font/Manager.hpp"

#include "Chicane/Core/Math/Curve.hpp"

namespace Chicane
{
    namespace Grid
    {
        static inline constexpr const char NULL_CHARACTER = '\0';

        static const Box::FontParsed EMPTY_FONT  = {};
        static const Box::FontGlyph  EMPTY_GLYPH = {};

        Character::Character()
            : Component(TAG_ID),
              m_bCanUpdate(false),
              m_character(NULL_CHARACTER)
        {}

        bool Character::isDrawable() const
        {
            return m_parent->isVisible() && hasGlyph() && hasPrimitive();
        }

        void Character::refreshPrimitive()
        {
            if (!m_bCanUpdate)
            {
                return;
            }

            if (!hasGlyph() || !Color::isVisible(m_style.foregroundColor))
            {
                clearPrimitive();

                return;
            }

            m_bCanUpdate = false;

            const Box::FontGlyph& glyph = getGlyph();

            Primitive primitive = {};
            primitive.indices   = glyph.indices;

            Vertex vertex = {};
            vertex.color  = m_style.foregroundColor;

            for (const Vec3 position : glyph.vertices)
            {
                vertex.position = position;

                primitive.vertices.push_back(vertex);
            }

            setPrimitive(primitive);
        }

        void Character::onRefresh()
        {
            refreshFont();
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

        const Box::FontParsed& Character::getFont() const
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

        const Box::FontGlyph& Character::getGlyph() const
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

            const Style& parentStyle = m_parent->getStyle();

            m_style.foregroundColor = parentStyle.foregroundColor;
            m_style.font            = parentStyle.font;

            if (!hasGlyph())
            {
                return;
            }

            const Box::FontGlyph& glyph = getGlyph();

            const float scale = m_style.font.size / Curve::FIXED_POINT / 2.0f;
            const float units = (m_style.font.size / glyph.units) * 1.25f;

            m_style.width  = glyph.line.x * scale;
            m_style.height = glyph.line.y * scale;

            m_style.margin.top = glyph.box.y * units;

            m_parent->addCursor(glyph.box.x * units, 0.0f);

            m_bCanUpdate = true;
        }
    }
}