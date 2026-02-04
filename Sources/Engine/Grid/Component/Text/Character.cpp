#include "Chicane/Grid/Component/Text/Character.hpp"

#include "Chicane/Box/Font/Manager.hpp"

#include "Chicane/Core/Math/Curve.hpp"

namespace Chicane
{
    namespace Grid
    {
        static constexpr inline const char NULL_CHARACTER = '\0';

        TextCharacter::TextCharacter()
            : Component(TAG_ID),
              m_character(NULL_CHARACTER),
              m_glyph({})
        {
            Box::getFontManager()->watch([&](const Box::FontManager::Instances& inData) { refreshFont(); });
        }

        bool TextCharacter::isDrawable() const
        {
            return m_parent->isVisible() && hasGlyph() && hasPrimitive();
        }

        void TextCharacter::refreshPrimitive()
        {
            const Box::FontGlyph& glyph = getGlyph();

            Primitive primitive = {};
            primitive.indices   = glyph.indices;

            Vertex vertex = {};
            vertex.color  = m_style.foregroundColor.get();

            for (const Vec3 position : glyph.vertices)
            {
                vertex.position = position;

                primitive.vertices.push_back(vertex);
            }

            setPrimitive(primitive);
        }

        void TextCharacter::onRefresh()
        {
            if (!hasParent() || !hasGlyph())
            {
                return;
            }

            m_style.background.color = m_parent->getStyle().foregroundColor;

            m_parent->addCursor(m_glyph.box.x * ((m_style.font.size.get() / m_glyph.units) * 1.25f), 0.0f);

            m_style.zIndex.set(getParent()->getStyle().zIndex.get() + 0.1f);
        }

        void TextCharacter::disable()
        {
            setCharacter(NULL_CHARACTER);
        }

        bool TextCharacter::hasCharacter() const
        {
            return m_character != NULL_CHARACTER;
        }

        char TextCharacter::getCharacter() const
        {
            return m_character;
        }

        void TextCharacter::setCharacter(char inValue)
        {
            if (m_character == inValue)
            {
                return;
            }

            setValue(m_character, inValue);

            refreshFont();
        }

        bool TextCharacter::hasGlyph() const
        {
            return m_glyph.indices.size() > 0;
        }

        const Box::FontGlyph& TextCharacter::getGlyph() const
        {
            return m_glyph;
        }

        void TextCharacter::refreshFont()
        {
            if (!hasParent())
            {
                clearPrimitive();

                m_glyph = {};

                return;
            }

            const Box::FontParsed& font = Box::getFontManager()->getFamily(m_style.font.family.get());

            if (!font.hasGlyph(m_character))
            {
                clearPrimitive();

                m_glyph = {};

                return;
            }

            m_glyph = font.getGlyph(m_character);

            const Style& parentStyle = m_parent->getStyle();

            m_style.font = parentStyle.font;

            const float scale = m_style.font.size.get() / Curve::FIXED_POINT / 2.0f;
            const float units = (m_style.font.size.get() / m_glyph.units) * 1.25f;

            m_style.width.set(m_glyph.line.x * scale);
            m_style.height.set(m_glyph.line.y * scale);

            m_style.margin.top = m_glyph.box.y * units;

            refreshPrimitive();
        }
    }
}