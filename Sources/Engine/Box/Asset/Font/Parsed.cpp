#include "Chicane/Box/Asset/Font/Parsed.hpp"

namespace Chicane
{
    namespace Box
    {
        namespace Font
        {
            static const Glyph EMPTY_GLYPH = {};

            bool Parsed::hasGlyph(char inCharacter) const
            {
                return hasGlyph(static_cast<std::uint32_t>(inCharacter));
            }

            bool Parsed::hasGlyph(std::uint32_t inCode) const
            {
                return glyphs.find(inCode) != glyphs.end();
            }

            const Glyph& Parsed::getGlyph(char inCharacter) const
            {
                return getGlyph(static_cast<std::uint32_t>(inCharacter));
            }

            const Glyph& Parsed::getGlyph(std::uint32_t inCode) const
            {
                if (!hasGlyph(inCode))
                {
                    return EMPTY_GLYPH;
                }

                return glyphs.at(inCode);
            }
        }
    }
}