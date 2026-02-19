#include "Chicane/Grid/Component/Text/Character.hpp"

#include "Chicane/Box/Font/Manager.hpp"

#include "Chicane/Core/Log.hpp"

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
            return m_parent->isDisplayable() && hasGlyph() && hasPrimitive();
        }

        void TextCharacter::refreshPrimitive()
        {
            const Box::FontGlyph& glyph = getGlyph();

            Primitive primitive = {};
            primitive.indices   = glyph.indices;

            Vertex vertex = {};

            for (const Vec3 position : glyph.vertices)
            {
                vertex.position = position;

                primitive.vertices.push_back(vertex);
            }

            setPrimitive(primitive);
        }

        void TextCharacter::onRefresh()
        {
            if (!hasParent())
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

        char32_t TextCharacter::getCharacter() const
        {
            return m_character;
        }

        void TextCharacter::setCharacter(char32_t inValue)
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
            return !m_glyph.vertices.empty();
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

            const Style& parentStyle = m_parent->getStyle();

            const Box::FontParsed& font = Box::getFontManager()->getFamily(parentStyle.font.family.get());

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

            if (!hasGlyph())
            {
                if (m_character == ' ')
                {
                    m_parent->addCursor(Box::Font::BASE_SIZE * 0.4f, 0.0f);
                }

                return;
            }

            m_primitive.scale = parentStyle.font.size.get();

            m_style.zIndex.set(parentStyle.zIndex.get() + 0.11f);
            m_style.background.color = parentStyle.foregroundColor;

            float scale    = parentStyle.font.size.get() * m_glyph.units;
            float width    = m_glyph.width * scale;
            float height   = m_glyph.height * scale;
            float advance  = m_glyph.advance * scale;
            float ascender = m_glyph.ascender * scale;
            Vec2  bearing  = m_glyph.bearing * scale;

            m_style.width.setRaw(std::to_string(width));
            m_style.height.setRaw(std::to_string(height));
            m_style.margin.left.setRaw(std::to_string(bearing.x));

            m_parent->addCursor(advance - width, 0.0f);
        }
    }
}