#pragma once

#include "Chicane/Box/Essential.hpp"
#include "Chicane/Box/Asset/Font/Glyph.hpp"

namespace Chicane
{
    namespace Box
    {
        namespace Font
        {
            struct CHICANE_BOX Parsed
            {
            public:
                bool hasGlyph(char inCharacter) const;
                bool hasGlyph(std::uint32_t inCode) const;

                const Glyph& getGlyph(char inCharacter) const;
                const Glyph& getGlyph(std::uint32_t inCode) const;

            public:
                String name   = "";
                Glyph::Map  glyphs = {};
            };
        }
    }
}