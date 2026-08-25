#include "Chicane/Box/Font/Family.hpp"

#include <cmath>

namespace Chicane
{
    namespace Box
    {
        FontFamily::FontFamily()
            : m_name(String::empty()),
              m_family(String::empty()),
              m_glyphs(),
              m_ascender(0.0f),
              m_descender(0.0f),
              m_weight(400.0f),
              m_weightMin(400.0f),
              m_weightMax(400.0f),
              m_kernings({})
        {}

        std::uint64_t FontFamily::kerningKey(char32_t inLeft, char32_t inRight)
        {
            return (static_cast<std::uint64_t>(inLeft) << 32) | static_cast<std::uint64_t>(inRight);
        }

        float FontFamily::getAscender() const
        {
            return m_ascender;
        }

        float FontFamily::getDescender() const
        {
            return m_descender;
        }

        float FontFamily::getLineHeight() const
        {
            return m_ascender - m_descender;
        }

        void FontFamily::setMetrics(float inAscender, float inDescender)
        {
            m_ascender  = inAscender;
            m_descender = inDescender;
        }

        float FontFamily::getKerning(char32_t inLeft, char32_t inRight) const
        {
            const auto it = m_kernings.find(kerningKey(inLeft, inRight));

            if (it == m_kernings.end())
            {
                return 0.0f;
            }

            return it->second;
        }

        void FontFamily::addKerning(char32_t inLeft, char32_t inRight, float inValue)
        {
            if (inValue == 0.0f)
            {
                return;
            }

            m_kernings[kerningKey(inLeft, inRight)] = inValue;
        }

        float FontFamily::getWeight() const
        {
            return m_weight;
        }

        void FontFamily::setWeight(float inValue)
        {
            m_weight = inValue;
        }

        float FontFamily::getWeightMin() const
        {
            return m_weightMin;
        }

        float FontFamily::getWeightMax() const
        {
            return m_weightMax;
        }

        void FontFamily::setWeightRange(float inMinimum, float inMaximum)
        {
            m_weightMin = inMinimum;
            m_weightMax = inMaximum;
        }

        bool FontFamily::isVariable() const
        {
            return m_weightMin < m_weightMax;
        }

        bool FontFamily::hasGlyph(char inCharacter) const
        {
            return hasGlyph(static_cast<char32_t>(inCharacter));
        }

        bool FontFamily::hasGlyph(char32_t inCode) const
        {
            return m_glyphs.find(inCode) != m_glyphs.end();
        }

        const String& FontFamily::getName() const
        {
            return m_name;
        }

        void FontFamily::setName(const String& inValue)
        {
            m_name = inValue;
        }

        const String& FontFamily::getFamily() const
        {
            return m_family;
        }

        void FontFamily::setFamily(const String& inValue)
        {
            m_family = inValue;
        }

        const FontFamily::Glyphs& FontFamily::getGlyphs() const
        {
            return m_glyphs;
        }

        const FontGlyph& FontFamily::getGlyph(char inCharacter) const
        {
            return getGlyph(static_cast<char32_t>(inCharacter));
        }

        const FontGlyph& FontFamily::getGlyph(char32_t inCode) const
        {
            if (!hasGlyph(inCode))
            {
                return FontGlyph::empty();
            }

            return m_glyphs.at(inCode);
        }

        void FontFamily::addGlyph(const Glyphs& inValue)
        {
            for (const auto& [code, glyph] : inValue)
            {
                addGlyph(glyph);
            }
        }

        void FontFamily::addGlyph(const FontGlyph& inValue)
        {
            FontGlyph glyph = inValue;
            glyph.name      = m_name;
            glyph.name.append('_');
            glyph.name.append(std::to_string(static_cast<int>(std::round(m_weight))));
            glyph.name.append('_');
            glyph.name.append(std::to_string(static_cast<int>(glyph.code)));

            m_glyphs[glyph.code] = glyph;
        }
    }
}