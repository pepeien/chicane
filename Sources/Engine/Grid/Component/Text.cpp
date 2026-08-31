#include "Chicane/Grid/Component/Text.reflected.hpp"

#include <algorithm>
#include <limits>

#include "Chicane/Core/Math/Vertex.hpp"
#include "Chicane/Grid/Component/Text/Glyph.hpp"

namespace Chicane
{
    namespace Grid
    {
        namespace
        {
            const Box::FontGlyph& resolveGlyph(const Box::FontFamily& inFamily, char32_t inCode)
            {
                if (inFamily.hasGlyph(inCode))
                {
                    return inFamily.getGlyph(inCode);
                }

                if (inFamily.hasGlyph(U'?'))
                {
                    return inFamily.getGlyph(U'?');
                }

                const Box::FontFamily::Glyphs& glyphs = inFamily.getGlyphs();
                if (!glyphs.empty())
                {
                    return glyphs.begin()->second;
                }

                return Box::FontGlyph::empty();
            }

            void getFontMetrics(const Box::FontFamily& inFamily, float& outAscender, float& outDescender)
            {
                outAscender  = inFamily.getAscender();
                outDescender = inFamily.getDescender();

                if (outAscender != 0.0f || outDescender != 0.0f)
                {
                    return;
                }

                const Box::FontFamily::Glyphs& glyphs = inFamily.getGlyphs();
                if (glyphs.empty())
                {
                    outAscender  = 1.0f;
                    outDescender = 0.0f;

                    return;
                }

                const Box::FontGlyph& reference = glyphs.begin()->second;
                outAscender                     = reference.ascender;
                outDescender                    = reference.descender;
            }
        }

        Text::Text(const pugi::xml_node& inNode)
            : Scrollable(inNode),
              m_text(""),
              m_parsedText(""),
              m_layoutSignature(""),
              m_font(nullptr),
              m_contentSize(Vec2::Zero()),
              m_glyphs({})
        {
            setText(inNode.text().as_string());

            Primitive primitive;
            primitive.indices = {0, 1, 2, 2, 3, 0};

            Vertex vertex = {};

            vertex.uv.x       = 0.0f;
            vertex.uv.y       = 0.0f;
            vertex.position.x = -0.5f;
            vertex.position.y = -0.5f;
            primitive.vertices.push_back(vertex);

            vertex.uv.x       = 1.0f;
            vertex.position.x = 0.5f;
            primitive.vertices.push_back(vertex);

            vertex.uv.y       = 1.0f;
            vertex.position.y = 0.5f;
            primitive.vertices.push_back(vertex);

            vertex.uv.x       = 0.0f;
            vertex.position.x = -0.5f;
            primitive.vertices.push_back(vertex);

            setPrimitive(primitive);

            addStyleProperties({
                {Style::DISPLAY_ATTRIBUTE_NAME,        Style::DISPLAY_TYPE_FLEX      },
                {Style::FLEX_DIRECTION_ATTRIBUTE_NAME, Style::FLEX_DIRECTION_TYPE_ROW},
                {Style::FONT_SIZE_ATTRIBUTE_NAME,      Style::FONT_SIZE_DEFAULT_VALUE}
            });
        }

        Text::~Text()
        {
            for (TextGlyph* glyph : m_glyphs)
            {
                delete glyph;
            }

            m_glyphs.clear();
        }

        void Text::refresh()
        {
            Component::refresh();

            syncGlyphs();
        }

        std::vector<Component*> Text::getChildrenFlat() const
        {
            std::vector<Component*> result = Scrollable::getChildrenFlat();

            for (TextGlyph* glyph : m_glyphs)
            {
                if (!glyph)
                {
                    continue;
                }

                result.push_back(glyph);
            }

            return result;
        }

        void Text::onRefresh()
        {
            if (!isDisplayable())
            {
                return;
            }

            refreshFont();
            refreshText();
        }

        const String& Text::getText() const
        {
            return m_text;
        }

        void Text::setText(const String& inValue)
        {
            if (inValue.equals(m_text))
            {
                return;
            }

            m_text = inValue;
        }

        const Vec2& Text::getContentSize() const
        {
            return m_contentSize;
        }

        Vec2 Text::getTransformPivot() const
        {
            Vec2 min;
            Vec2 max;

            if (!getGlyphVisualBounds(min, max))
            {
                return Component::getTransformPivot();
            }

            const Vec2 visualSize(max.x - min.x, max.y - min.y);

            return getDrawPosition() + min + getStyle().getTransformOrigin(visualSize);
        }

        bool Text::hasFont() const
        {
            return m_font != nullptr;
        }

        bool Text::getGlyphVisualBounds(Vec2& outMin, Vec2& outMax) const
        {
            bool hasGlyph = false;

            outMin = Vec2(std::numeric_limits<float>::max(), std::numeric_limits<float>::max());
            outMax = Vec2(std::numeric_limits<float>::lowest(), std::numeric_limits<float>::lowest());

            const Vec2 padding(m_style.insetLeft(), m_style.insetTop());

            for (const TextGlyph* glyph : m_glyphs)
            {
                if (!glyph)
                {
                    continue;
                }

                const Vec2 scale = glyph->getScale();

                if (scale.x <= 0.0f && scale.y <= 0.0f)
                {
                    continue;
                }

                const Vec2 size   = glyph->getSize();
                const Vec2 offset = glyph->getOffset();
                const Vec2 center(
                    padding.x + glyph->getRelativeX() + (size.x * 0.5f) + offset.x,
                    padding.y + glyph->getRelativeY() + (size.y * 0.5f) - offset.y
                );
                const Vec2 half(scale.x * 0.5f, scale.y * 0.5f);

                outMin.x = std::min(outMin.x, center.x - half.x);
                outMin.y = std::min(outMin.y, center.y - half.y);
                outMax.x = std::max(outMax.x, center.x + half.x);
                outMax.y = std::max(outMax.y, center.y + half.y);
                hasGlyph = true;
            }

            return hasGlyph;
        }

        void Text::refreshFont()
        {
            m_font = Box::findFont(m_style.font.family.get(), m_style.font.weight.get());
        }

        TextGlyph* Text::acquireGlyph(std::size_t inIndex)
        {
            while (m_glyphs.size() <= inIndex)
            {
                TextGlyph* glyph = new TextGlyph();
                glyph->setRoot(m_root);
                glyph->setParent(this);
                glyph->setStyleFile(m_styleFile);

                m_glyphs.push_back(glyph);
            }

            return m_glyphs.at(inIndex);
        }

        void Text::refreshSize()
        {
            if (isRoot())
            {
                return;
            }

            applyContentSize();
        }

        void Text::applyContentSize()
        {
            setSize(m_contentSize.x, m_contentSize.y);
        }

        void Text::refreshText()
        {
            if (!hasFont())
            {
                m_contentSize = Vec2::Zero();
                applyContentSize();

                return;
            }

            const String      value         = parseText(m_text);
            const float       fontSize      = m_style.font.size.get();
            const float       letterSpacing = m_style.letterSpacing.get();
            const Color::Rgba color         = m_style.foregroundColor.get();
            const String      signature =
                value + "|" + m_style.font.family.get() + "|" + std::to_string(m_style.font.weight.get()) + "|" +
                std::to_string(fontSize) + "|" + std::to_string(letterSpacing) + "|" + std::to_string(color.r) + "|" +
                std::to_string(color.g) + "|" + std::to_string(color.b) + "|" + std::to_string(color.a);

            if (signature.equals(m_layoutSignature))
            {
                applyContentSize();

                return;
            }

            m_layoutSignature = signature;
            m_parsedText      = value;

            const Box::FontFamily& fontFamily = m_font->getData(m_style.font.weight.get());
            float                  ascender   = 0.0f;
            float                  descender  = 0.0f;

            getFontMetrics(fontFamily, ascender, descender);

            const float                 lineHeight = (ascender - descender) * fontSize;
            const std::vector<char32_t> codepoints = value.toUnicode();

            float       cursorX      = 0.0f;
            float       maxWidth     = 0.0f;
            std::size_t lineCount    = 1;
            std::size_t glyphIndex   = 0;
            char32_t    previousCode = 0U;
            bool        hasPrevious  = false;

            for (std::size_t i = 0; i < codepoints.size(); ++i)
            {
                const char32_t codepoint = codepoints.at(i);

                if (codepoint == U'\n')
                {
                    maxWidth = std::max(maxWidth, cursorX);
                    cursorX  = 0.0f;
                    lineCount++;
                    hasPrevious = false;

                    continue;
                }

                const Box::FontGlyph& glyph = resolveGlyph(fontFamily, codepoint);

                if (glyph.name.isEmpty())
                {
                    continue;
                }

                if (hasPrevious)
                {
                    cursorX += fontFamily.getKerning(previousCode, codepoint) * fontSize;
                }

                TextGlyph* textGlyph = acquireGlyph(glyphIndex++);
                textGlyph->configure(
                    &glyph,
                    fontSize,
                    letterSpacing,
                    color,
                    cursorX,
                    (lineCount - 1) * lineHeight,
                    ascender
                );

                cursorX += textGlyph->getAdvance();
                maxWidth = std::max(maxWidth, cursorX);

                previousCode = codepoint;
                hasPrevious  = true;
            }

            for (std::size_t i = glyphIndex; i < m_glyphs.size(); ++i)
            {
                m_glyphs.at(i)->clear();
            }

            m_contentSize = {maxWidth, lineCount * lineHeight};
            m_style.width.set(m_contentSize.x);
            m_style.height.set(m_contentSize.y);
            applyContentSize();
        }

        void Text::syncGlyphs()
        {
            for (TextGlyph* glyph : m_glyphs)
            {
                if (!glyph)
                {
                    continue;
                }

                glyph->syncPosition();
            }
        }
    }
}
