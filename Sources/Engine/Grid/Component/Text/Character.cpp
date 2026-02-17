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
            Primitive primitive = {};
            primitive.indices   = m_glyph.indices;

            Vertex vertex = {};

            for (const Vec3 position : m_glyph.vertices)
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

            refreshFontStyle();
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

            m_character = inValue;

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

            refreshPrimitive();
        }

        void TextCharacter::refreshFontStyle()
        {
            const Style& parentStyle = getParent()->getStyle();

            m_style.zIndex.set(parentStyle.zIndex.get());
            m_style.background.color = parentStyle.foregroundColor;
            m_style.font             = parentStyle.font;

            const float scale = m_style.font.size.get() / m_glyph.units;
            m_style.width.set(m_glyph.units * scale);
            m_style.height.set(m_glyph.units * scale);
            m_style.margin.right.set(m_glyph.width * scale);
        }
    }
}