#include "Chicane/Grid/Component/Text.reflected.hpp"

#include <algorithm>
#include <limits>

#include "Chicane/Core/Math/Vertex.hpp"
#include "Chicane/Core/Size.hpp"
#include "Chicane/Grid/Component/Text/Glyph.hpp"

namespace Chicane
{
    namespace Grid
    {
        static const Box::FontGlyph& resolveGlyph(const Box::FontFamily& inFamily, char32_t inCode)
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

            return Box::FontGlyph::sEmpty();
        }

        static void getFontMetrics(const Box::FontFamily& inFamily, float& outAscender, float& outDescender)
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

        static bool isBreakableSpace(char32_t inCode)
        {
            return inCode == U' ' || inCode == U'\t' || inCode == U'\r';
        }

        static float glyphAdvance(
            const Box::FontFamily& inFamily,
            char32_t               inCode,
            char32_t               inPrevious,
            bool                   bHasPrevious,
            float                  inFontSize,
            float                  inLetterSpacing
        )
        {
            const Box::FontGlyph& glyph = resolveGlyph(inFamily, inCode);
            if (glyph.name.isEmpty())
            {
                return 0.0f;
            }

            float advance = (glyph.advance + inLetterSpacing) * inFontSize;
            if (bHasPrevious)
            {
                advance += inFamily.getKerning(inPrevious, inCode) * inFontSize;
            }

            return advance;
        }

        static float measureWord(
            const Box::FontFamily&       inFamily,
            const std::vector<char32_t>& inCodes,
            std::size_t                  inStart,
            char32_t                     inPrevious,
            bool                         bHasPrevious,
            float                        inFontSize,
            float                        inLetterSpacing
        )
        {
            float    width       = 0.0f;
            char32_t previous    = inPrevious;
            bool     hasPrevious = bHasPrevious;

            for (std::size_t i = inStart; i < inCodes.size(); i++)
            {
                const char32_t code = inCodes.at(i);
                if (code == U'\n' || isBreakableSpace(code))
                {
                    break;
                }

                const float advance = glyphAdvance(inFamily, code, previous, hasPrevious, inFontSize, inLetterSpacing);
                if (advance <= 0.0f)
                {
                    continue;
                }

                width += advance;
                previous    = code;
                hasPrevious = true;
            }

            return width;
        }

        Text::Text(const XmlNode& inNode)
            : Scrollable(inNode),
              m_text(""),
              m_parsedText(""),
              m_layoutSignature(""),
              m_font(nullptr),
              m_contentSize(Vec2::sZero()),
              m_glyphs({})
        {
            setText(inNode.getText());

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

            if (m_style.isDisplay(StyleDisplay::None))
            {
                return;
            }

            syncGlyphs();
        }

        void Text::refreshPeripherals()
        {
            std::vector<Component*> peripherals(m_glyphs.begin(), m_glyphs.end());

            setPeripherals(peripherals);
        }

        void Text::onRefresh()
        {
            if (!isDisplayable())
            {
                return;
            }

            if (!isReference(m_text) && !hasFlag(ComponentDirty::LaidOut) && !m_layoutSignature.isEmpty())
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

            m_text            = inValue;
            m_layoutSignature = String::sEmpty();
            markLayoutDirty();
        }

        const Vec2& Text::getContentSize() const
        {
            return m_contentSize;
        }

        std::size_t Text::getGlyphCount() const
        {
            std::size_t count = 0;

            for (const TextGlyph* glyph : m_glyphs)
            {
                if (glyph && glyph->isLive())
                {
                    count++;
                }
            }

            return count;
        }

        bool Text::getGlyphVisualBounds(Vec2& outMin, Vec2& outMax) const
        {
            bool hasGlyph = false;

            outMin = Vec2(std::numeric_limits<float>::max(), std::numeric_limits<float>::max());
            outMax = Vec2(std::numeric_limits<float>::lowest(), std::numeric_limits<float>::lowest());

            const Vec2 padding(m_style.insetLeft(), m_style.insetTop());

            for (const TextGlyph* glyph : m_glyphs)
            {
                if (!glyph || !glyph->isLive())
                {
                    continue;
                }

                const Vec2 scale = glyph->getScale();

                if (scale.x <= 0.0f && scale.y <= 0.0f)
                {
                    continue;
                }

                const Vec2 size     = glyph->getSize();
                const Vec2 offset   = glyph->getOffset();
                const Vec2 relative = glyph->getRelative();
                const Vec2 center(
                    padding.x + relative.x + (size.x * 0.5f) + offset.x,
                    padding.y + relative.y + (size.y * 0.5f) - offset.y
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

        float Text::getInsertionX(std::size_t inIndex) const
        {
            std::size_t live = 0;
            float       endX = 0.0f;

            for (const TextGlyph* glyph : m_glyphs)
            {
                if (!glyph || !glyph->isLive())
                {
                    continue;
                }

                const float left  = glyph->getRelative().x;
                const float right = left + glyph->getAdvance();

                if (live == 0)
                {
                    endX = left;
                }

                if (live == inIndex)
                {
                    return left;
                }

                endX = right;
                live++;
            }

            return endX;
        }

        std::size_t Text::getInsertionIndexAt(float inLocalX) const
        {
            std::size_t live = 0;

            for (const TextGlyph* glyph : m_glyphs)
            {
                if (!glyph || !glyph->isLive())
                {
                    continue;
                }

                const float left    = glyph->getRelative().x;
                const float advance = glyph->getAdvance();
                const float mid     = left + (advance * 0.5f);

                if (inLocalX < mid)
                {
                    return live;
                }

                live++;
            }

            return live;
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

            const bool bIsWidthAuto  = m_style.width.isAuto();
            const bool bIsHeightAuto = m_style.height.isAuto();

            if (!bIsWidthAuto || !bIsHeightAuto)
            {
                Component::refreshSize();
            }

            applyContentSize();
        }

        void Text::applyContentSize()
        {
            const bool bIsWidthAuto  = m_style.width.isAuto();
            const bool bIsHeightAuto = m_style.height.isAuto();

            if (!bIsWidthAuto && !bIsHeightAuto)
            {
                return;
            }

            setSize(
                bIsWidthAuto ? m_contentSize.x + m_style.insetHorizontal() : m_size.x,
                bIsHeightAuto ? m_contentSize.y + m_style.insetVertical() : m_size.y
            );
        }

        void Text::refreshPosition()
        {
            Component::refreshPosition();

            syncGlyphs();
        }

        void Text::refreshText()
        {
            if (!hasFont())
            {
                m_contentSize = Vec2::sZero();

                applyContentSize();

                return;
            }

            const String         value         = parseText(m_text);
            const float          fontSize      = m_style.font.size.get();
            const float          letterSpacing = m_style.letterSpacing.get();
            const Color::Rgba    color         = m_style.foregroundColor.get();
            const float          innerWidth    = std::max(0.0f, m_size.x - m_style.insetHorizontal());
            const StyleWordBreak wordBreak     = m_style.wordBreak.get();

            float wrapWidth = 0.0f;
            auto  tighten   = [&wrapWidth](float inWidth)
            {
                if (inWidth <= 0.0f)
                {
                    return;
                }

                wrapWidth = wrapWidth > 0.0f ? std::min(wrapWidth, inWidth) : inWidth;
            };

            if (!m_style.width.isAuto())
            {
                tighten(innerWidth);
            }

            const float ownInset = m_style.insetHorizontal();
            if (m_style.width.hasMax())
            {
                tighten(m_style.width.max.get() - ownInset);
            }

            if (hasParent())
            {
                const Style& parentStyle = m_parent->getStyle();
                const float  parentInset = parentStyle.insetHorizontal();
                const float  childGutter = ownInset + m_style.margin.left.get() + m_style.margin.right.get();

                if (!parentStyle.width.isAuto())
                {
                    tighten(m_parent->getSize().x - parentInset - childGutter);
                }

                if (parentStyle.width.hasMax())
                {
                    tighten(parentStyle.width.max.get() - parentInset - childGutter);
                }
            }

            const String signature =
                value + "|" + m_style.font.family.get() + "|" + std::to_string(m_style.font.weight.get()) + "|" +
                std::to_string(fontSize) + "|" + std::to_string(letterSpacing) + "|" + std::to_string(color.r) + "|" +
                std::to_string(color.g) + "|" + std::to_string(color.b) + "|" + std::to_string(color.a) + "|" +
                std::to_string(static_cast<int>(m_style.align.get())) + "|" +
                std::to_string(static_cast<int>(wordBreak)) + "|" + std::to_string(innerWidth) + "|" +
                std::to_string(wrapWidth);

            if (signature.equals(m_layoutSignature))
            {
                return;
            }

            m_layoutSignature = signature;
            m_parsedText      = value;

            const std::size_t      glyphCount = m_glyphs.size();
            const Box::FontFamily& fontFamily = m_font->getData(m_style.font.weight.get());
            float                  ascender   = 0.0f;
            float                  descender  = 0.0f;

            getFontMetrics(fontFamily, ascender, descender);

            const float                 lineHeight = (ascender - descender) * fontSize;
            const std::vector<char32_t> codepoints = value.toUnicode();

            Vec2        cursor       = Vec2::sZero();
            float       maxWidth     = 0.0f;
            std::size_t lineCount    = 1;
            std::size_t glyphIndex   = 0;
            std::size_t lineStart    = 0;
            char32_t    previousCode = 0U;
            bool        hasPrevious  = false;

            auto flushLine = [&]()
            {
                applyTextAlignment(lineStart, glyphIndex, cursor.x);

                lineStart = glyphIndex;
            };

            for (std::size_t i = 0; i < codepoints.size(); i++)
            {
                const char32_t codepoint = codepoints.at(i);

                if (codepoint == U'\n')
                {
                    maxWidth = std::max(maxWidth, cursor.x);

                    flushLine();

                    cursor.x = 0.0f;

                    lineCount++;

                    hasPrevious = false;

                    continue;
                }

                const Box::FontGlyph& glyph = resolveGlyph(fontFamily, codepoint);

                if (glyph.name.isEmpty())
                {
                    continue;
                }

                const float kerning = hasPrevious ? fontFamily.getKerning(previousCode, codepoint) * fontSize : 0.0f;
                const float advance = (glyph.advance + letterSpacing) * fontSize;
                const float nextX   = cursor.x + kerning + advance;

                bool bWrap = false;
                if (wordBreak != StyleWordBreak::Normal && wrapWidth > 0.0f && cursor.x > 0.0f)
                {
                    if (wordBreak == StyleWordBreak::BreakAll)
                    {
                        bWrap = nextX > wrapWidth;
                    }
                    else
                    {
                        const bool bWordStart =
                            !isBreakableSpace(codepoint) && (!hasPrevious || isBreakableSpace(previousCode));

                        if (bWordStart)
                        {
                            const float wordWidth = measureWord(
                                fontFamily,
                                codepoints,
                                i,
                                previousCode,
                                hasPrevious,
                                fontSize,
                                letterSpacing
                            );

                            if (cursor.x + wordWidth > wrapWidth)
                            {
                                bWrap = wordWidth <= wrapWidth || nextX > wrapWidth;
                            }
                        }
                        else
                        {
                            bWrap = nextX > wrapWidth;
                        }
                    }
                }

                if (bWrap)
                {
                    maxWidth = std::max(maxWidth, cursor.x);

                    flushLine();

                    cursor.x = 0.0f;
                    lineCount++;
                    hasPrevious = false;

                    if (isBreakableSpace(codepoint))
                    {
                        continue;
                    }
                }

                if (hasPrevious)
                {
                    cursor.x += fontFamily.getKerning(previousCode, codepoint) * fontSize;
                }

                cursor.y = (lineCount - 1) * lineHeight;

                TextGlyph* textGlyph = acquireGlyph(glyphIndex++);
                textGlyph->configure(&glyph, fontSize, letterSpacing, color, cursor, ascender);

                cursor.x += textGlyph->getAdvance();
                maxWidth = std::max(maxWidth, cursor.x);

                previousCode = codepoint;
                hasPrevious  = true;
            }

            maxWidth = std::max(maxWidth, cursor.x);

            for (std::size_t i = glyphIndex; i < m_glyphs.size(); i++)
            {
                m_glyphs.at(i)->clear();
            }

            if (m_glyphs.size() != glyphCount)
            {
                refreshPeripherals();
            }

            m_contentSize = {maxWidth, lineCount * lineHeight};

            applyContentSize();
            flushLine();
        }

        void Text::applyTextAlignment(std::size_t inStart, std::size_t inEnd, float inLineWidth)
        {
            if (inStart >= inEnd || inLineWidth <= 0.0f)
            {
                return;
            }

            const float inner = std::max(0.0f, m_size.x - m_style.insetHorizontal());
            float       shift = 0.0f;

            switch (m_style.align.get())
            {
            case StyleAlignment::Center:
                shift = std::max(0.0f, (inner - inLineWidth) * 0.5f);

                break;

            case StyleAlignment::End:
                shift = std::max(0.0f, inner - inLineWidth);

                break;

            default:
                return;
            }

            if (shift <= 0.0f)
            {
                return;
            }

            for (std::size_t i = inStart; i < inEnd && i < m_glyphs.size(); i++)
            {
                TextGlyph* glyph = m_glyphs.at(i);
                if (!glyph)
                {
                    continue;
                }

                const Vec2 relative = glyph->getRelative();

                glyph->setRelative(Vec2(relative.x + shift, relative.y));
            }
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
