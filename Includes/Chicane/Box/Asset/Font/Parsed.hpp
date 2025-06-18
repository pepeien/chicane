#pragma once

#include "Chicane/Box/Essential.hpp"
#include "Chicane/Box/Asset/Font/Glyph.hpp"

namespace Chicane
{
    namespace Box
    {
        struct CHICANE_BOX FontParsed
        {
        public:
            bool hasGlyph(char inCharacter) const;
            bool hasGlyph(std::uint32_t inCode) const;

            const FontGlyph& getGlyph(char inCharacter) const;
            const FontGlyph& getGlyph(std::uint32_t inCode) const;

        public:
            String          name   = "";
            FontGlyph::Map  glyphs = {};
        };
    }
}