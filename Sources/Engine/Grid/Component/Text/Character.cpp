#include "Chicane/Grid/Component/Text/Character.hpp"

#include "Chicane/Box/Font.hpp"

namespace Chicane
{
    namespace Grid
    {
        TextCharacter::TextCharacter()
            : Component(TAG_ID),
              m_glyph(nullptr)
        {}

        bool TextCharacter::isDrawable() const
        {
            return m_parent->isDisplayable() && hasGlyph() && hasPrimitive();
        }

        void TextCharacter::refreshPrimitive()
        {
            if (!hasParent() || !hasGlyph())
            {
                clearPrimitive();

                return;
            }

            Primitive primitive = {};
            primitive.indices   = m_glyph->indices;
            primitive.vertices  = m_glyph->vertices;

            setPrimitive(primitive);
        }

        void TextCharacter::onRefresh()
        {
            refreshGlyphStyle();
        }

        bool TextCharacter::hasGlyph() const
        {
            return m_glyph != nullptr;
        }

        const Box::FontGlyph* TextCharacter::getGlyph() const
        {
            return m_glyph;
        }

        void TextCharacter::setGlyph(const Box::FontGlyph* inValue)
        {
            m_glyph = inValue;

            refreshGlyphStyle();
            refreshPrimitive();
        }

        void TextCharacter::refreshGlyphStyle()
        {
            if (!hasParent() || !hasGlyph())
            {
                return;
            }

            const Style& parentStyle = getParent()->getStyle();

            m_style.background.color = parentStyle.foregroundColor;

            const float scale      = parentStyle.font.size.get();
            const float width      = m_glyph->width * scale;
            const float height     = m_glyph->height * scale;
            const float advance    = m_glyph->advance * scale;
            const float ascender   = m_glyph->ascender * scale;
            const float descender  = m_glyph->descender * scale;
            const float lineHeight = m_glyph->lineHeight * scale;
            const Vec2  bearing    = m_glyph->bearing * scale;

            m_style.width.set(width);
            m_style.height.set(height);

            m_style.margin.left.set(bearing.x);
            m_style.margin.right.set(advance - width);
            m_style.margin.top.set(bearing.y - (ascender * 0.25f));
            m_style.margin.bottom.set(descender);

            setScale(scale, scale);

            m_parent->addCursor(width * -0.5f, 0.0f);
        }
    }
}