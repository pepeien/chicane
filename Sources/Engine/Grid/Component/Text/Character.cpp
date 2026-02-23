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

            Vertex vertex = {};
            for (const Vec3 position : m_glyph->vertices)
            {
                vertex.position = position;

                primitive.vertices.push_back(vertex);
            }

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

        void TextCharacter::setGlyph(const Box::FontGlyph* inValue)
        {
            m_glyph = inValue;

            refreshPrimitive();
        }

        void TextCharacter::refreshGlyphStyle()
        {
            if (!hasParent() || !hasGlyph())
            {
                return;
            }

            const Style& parentStyle = getParent()->getStyle();

            m_primitive.scale = parentStyle.font.size.get();

            m_style.zIndex.setRaw(std::to_string(parentStyle.zIndex.get() + 1.0f));
            m_style.background.color = parentStyle.foregroundColor;

            float scale    = parentStyle.font.size.get() * m_glyph->units;
            float width    = m_glyph->width * scale;
            float height   = m_glyph->height * scale;
            float advance  = m_glyph->advance * scale;
            float ascender = m_glyph->ascender * scale;
            Vec2  bearing  = m_glyph->bearing * scale;

            m_style.width.setRaw(std::to_string(width));
            m_style.height.setRaw(std::to_string(height));
            m_style.margin.left.setRaw(std::to_string(bearing.x));

            m_parent->addCursor(advance - width, 0.0f);
        }
    }
}