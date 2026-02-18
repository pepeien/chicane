#include "Chicane/Box/Font/Parsed.hpp"

namespace Chicane
{
    namespace Box
    {
        bool FontParsed::hasGlyph(char inCharacter) const
        {
            return hasGlyph(static_cast<char32_t>(inCharacter));
        }

        bool FontParsed::hasGlyph(char32_t inCode) const
        {
            return glyphs.find(inCode) != glyphs.end();
        }

        const FontGlyph& FontParsed::getGlyph(char inCharacter) const
        {
            return getGlyph(static_cast<char32_t>(inCharacter));
        }

        const FontGlyph& FontParsed::getGlyph(char32_t inCode) const
        {
            if (!hasGlyph(inCode))
            {
                return FontGlyph::empty();
            }

            return glyphs.at(inCode);
        }
    }
}