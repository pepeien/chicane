#include "Chicane/Grid/Component/Character.hpp"

#include "Chicane/Box/Font/Manager.hpp"

#include "Chicane/Core/Math/Curve.hpp"
#include "Chicane/Core/Log.hpp"

namespace Chicane
{
    namespace Grid
    {
        static inline constexpr const char NULL_CHARACTER = '\0';

        Character::Character()
            : Component(TAG_ID),
              m_bCanUpdatePrimitive(false),
              m_character(NULL_CHARACTER),
              m_glyph({})
        {
            Box::getFontManager()->watchInstances([&](const Box::FontManager::Instances& inData) { refreshFont(); });
        }

        bool Character::isDrawable() const
        {
            return m_parent->isVisible() && hasGlyph() && hasPrimitive();
        }

        void Character::refreshPrimitive()
        {
            if (!m_bCanUpdatePrimitive)
            {
                return;
            }

            if (!hasGlyph() || !Color::isVisible(m_style.foregroundColor))
            {
                clearPrimitive();

                return;
            }

            m_bCanUpdatePrimitive = false;

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
            if (!hasParent() || !hasGlyph())
            {
                return;
            }

            m_parent->addCursor(m_glyph.box.x * ((m_style.font.size / m_glyph.units) * 1.25f), 0.0f);
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

            refreshFont();
        }

        bool Character::hasGlyph() const
        {
            return m_glyph.indices.size() > 0;
        }

        const Box::FontGlyph& Character::getGlyph() const
        {
            return m_glyph;
        }

        void Character::refreshFont()
        {
            if (!hasParent())
            {
                m_glyph = {};

                return;
            }

            const Box::FontParsed& font = Box::getFontManager()->getByFamily(m_style.font.family);

            if (!font.hasGlyph(m_character))
            {
                m_glyph = {};

                return;
            }

            m_glyph = font.getGlyph(m_character);

            if (!hasGlyph())
            {
                return;
            }

            const Style& parentStyle = m_parent->getStyle();

            m_style.foregroundColor = parentStyle.foregroundColor;
            m_style.font            = parentStyle.font;

            const float scale = m_style.font.size / Curve::FIXED_POINT / 2.0f;
            const float units = (m_style.font.size / m_glyph.units) * 1.25f;

            m_style.width  = m_glyph.line.x * scale;
            m_style.height = m_glyph.line.y * scale;
            m_style.zIndex = parentStyle.zIndex + 0.1f;

            m_style.margin.top = m_glyph.box.y * units;

            m_bCanUpdatePrimitive = true;
        }
    }
}