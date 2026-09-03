#include "Chicane/Grid/Component/Text/Glyph.hpp"

namespace Chicane
{
    namespace Grid
    {
        TextGlyph::TextGlyph()
            : Component(TAG_ID),
              m_glyph(nullptr),
              m_advance(0.0f),
              m_dilation(0.0f),
              m_relativeX(0.0f),
              m_relativeY(0.0f)
        {}

        bool TextGlyph::isDrawable() const
        {
            if (!hasParent() || !m_parent->isDisplayable() || m_parent->isCulled() || m_glyph == nullptr ||
                m_glyph->curves.empty())
            {
                return false;
            }

            return isVisible() && isSolid() && hasPrimitive();
        }

        void TextGlyph::refreshPrimitive()
        {
            if (m_glyph == nullptr || m_glyph->curves.empty())
            {
                clearPrimitive();

                return;
            }

            Primitive primitive;
            primitive.reference = QUAD_ID;
            primitive.glyph     = m_glyph->name;
            primitive.dilation  = m_dilation;
            primitive.indices   = {0, 1, 2, 2, 3, 0};

            Vertex vertex;

            // Bottom Left
            vertex.uv.x       = 0.0f;
            vertex.uv.y       = 0.0f;
            vertex.position.x = -0.5f;
            vertex.position.y = -0.5f;
            primitive.vertices.push_back(vertex);

            // Bottom Right
            vertex.uv.x       = 1.0f;
            vertex.uv.y       = 0.0f;
            vertex.position.x = 0.5f;
            vertex.position.y = -0.5f;
            primitive.vertices.push_back(vertex);

            // Top Right
            vertex.uv.x       = 1.0f;
            vertex.uv.y       = 1.0f;
            vertex.position.x = 0.5f;
            vertex.position.y = 0.5f;
            primitive.vertices.push_back(vertex);

            // Top Left
            vertex.uv.x       = 0.0f;
            vertex.uv.y       = 1.0f;
            vertex.position.x = -0.5f;
            vertex.position.y = 0.5f;
            primitive.vertices.push_back(vertex);

            setPrimitive(primitive);
        }

        void TextGlyph::refreshStyleRuleset()
        {
            return;
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

            if (m_glyph == nullptr || inFontSize <= 0.0f)
            {
                clear();

                return;
            }

            m_style.background.color.set(inColor);

            const float scale    = inFontSize;
            const float height   = m_glyph->height * scale;
            const float advance  = (m_glyph->advance + inLetterSpacing) * scale;
            const float ascender = inAscender * scale;

            m_dilation = QUAD_DILATION / scale;

            const Vec2 min(m_glyph->boundsMin.x - m_dilation, m_glyph->boundsMin.y - m_dilation);
            const Vec2 max(m_glyph->boundsMax.x + m_dilation, m_glyph->boundsMax.y + m_dilation);

            setScale((max.x - min.x) * scale, (max.y - min.y) * scale);
            setSize(advance, height);
            setOffset(
                ((min.x + max.x) * 0.5f * scale) - (advance * 0.5f),
                (height * 0.5f) - ascender + ((min.y + max.y) * 0.5f * scale)
            );

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

        float TextGlyph::getRelativeX() const
        {
            return m_relativeX;
        }

        float TextGlyph::getRelativeY() const
        {
            return m_relativeY;
        }

        void TextGlyph::clear()
        {
            m_glyph     = nullptr;
            m_advance   = 0.0f;
            m_dilation  = 0.0f;
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
                parent->getPosition().x + parentStyle.insetLeft() + m_relativeX,
                parent->getPosition().y + parentStyle.insetTop() + m_relativeY
            );

            refreshBounds();
        }
    }
}
