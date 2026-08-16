#include "Chicane/Grid/Component/Text/Glyph.hpp"

namespace Chicane
{
    namespace Grid
    {
        TextGlyph::TextGlyph()
            : Component(TAG_ID),
              m_glyph(nullptr),
              m_advance(0.0f),
              m_relativeX(0.0f),
              m_relativeY(0.0f)
        {}

        bool TextGlyph::isDrawable() const
        {
            if (!hasParent() || !m_parent->isDisplayable() || m_glyph == nullptr)
            {
                return false;
            }

            return isVisible() && isSolid() && hasPrimitive();
        }

        void TextGlyph::refreshPrimitive()
        {
            if (m_glyph == nullptr)
            {
                clearPrimitive();

                return;
            }

            Primitive primitive;
            primitive.reference = m_glyph->name;

            setPrimitive(primitive);
        }

        void TextGlyph::configure(
            const Box::FontGlyph* inGlyph,
            float                 inFontSize,
            float                 inLetterSpacing,
            const Color::Rgba&    inColor,
            float                 inPenX,
            float                 inLineTop,
            float                 inAscender
        )
        {
            m_glyph = inGlyph;

            if (m_glyph == nullptr)
            {
                clear();

                return;
            }

            m_style.background.color.set(inColor);

            const float scale    = inFontSize;
            const float height   = m_glyph->height * scale;
            const float advance  = (m_glyph->advance + inLetterSpacing) * scale;
            const float ascender = inAscender * scale;

            setScale(scale, scale);
            setSize(advance, height);
            setOffset(advance * -0.5f, (height * 0.5f) - ascender);

            m_style.width.set(advance);
            m_style.height.set(height);

            m_advance   = advance;
            m_relativeX = inPenX;
            m_relativeY = inLineTop;

            refreshPrimitive();
        }

        float TextGlyph::getAdvance() const
        {
            return m_advance;
        }

        void TextGlyph::clear()
        {
            m_glyph     = nullptr;
            m_advance   = 0.0f;
            m_relativeX = 0.0f;
            m_relativeY = 0.0f;

            clearPrimitive();
            setSize(0.0f, 0.0f);
        }

        void TextGlyph::syncPosition()
        {
            if (!hasParent())
            {
                return;
            }

            const Component* parent      = getParent();
            const Style&     parentStyle = parent->getStyle();

            setPosition(
                parent->getPosition().x + parentStyle.padding.left.get() + m_relativeX,
                parent->getPosition().y + parentStyle.padding.top.get() + m_relativeY
            );

            refreshBounds();
        }
    }
}
