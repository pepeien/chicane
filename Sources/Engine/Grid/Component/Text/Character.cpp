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

            Primitive primitive;
            primitive.reference = m_glyph->name;

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

            const float scale     = parentStyle.font.size.get();
            const float width     = m_glyph->width * scale;
            const float height    = m_glyph->height * scale;
            const float advance   = m_glyph->advance * scale;
            const float ascender  = m_glyph->ascender * scale;
            const float descender = m_glyph->descender * scale;
            const Vec2  bearing   = m_glyph->bearing * scale;

            const float baseline = descender - ascender;

            setScale(scale, scale);
            setOffset(-width, baseline + bearing.y);

            m_style.width.set(advance);
            m_style.height.set(height);

            const float widthOffset = m_style.width.get() * -0.5f;
            m_style.margin.right.set(widthOffset);

            const float heightOffset = m_style.height.get() * -0.5f;
            const float topOffset    = bearing.y * -0.5f;
            m_style.margin.top.set(heightOffset - topOffset - (ascender * 0.25f)); // I don't even know anymore
        }
    }
}