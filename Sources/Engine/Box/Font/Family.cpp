#include "Chicane/Box/Font/Family.hpp"

namespace Chicane
{
    namespace Box
    {
        FontFamily::FontFamily()
            : m_name(String::empty()),
              m_glyphs()
        {}

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
            glyph.name.append(std::to_string(static_cast<int>(glyph.code)));

            m_glyphs[glyph.code] = glyph;
        }
    }
}