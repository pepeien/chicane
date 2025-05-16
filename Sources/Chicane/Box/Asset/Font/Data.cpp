#include "Chicane/Box/Asset/Font/Data.hpp"

namespace Chicane
{
    namespace Box
    {
        namespace Font
        {
            static const Glyph EMPTY_GLYPH = {};

            bool ParsedData::hasGlyph(char inCharacter) const
            {
                return hasGlyph(static_cast<std::uint32_t>(inCharacter));
            }

            bool ParsedData::hasGlyph(std::uint32_t inCode) const
            {
                return glyphs.find(inCode) != glyphs.end();
            }

            const Glyph& ParsedData::getGlyph(char inCharacter) const
            {
                return getGlyph(static_cast<std::uint32_t>(inCharacter));
            }

            const Glyph& ParsedData::getGlyph(std::uint32_t inCode) const
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