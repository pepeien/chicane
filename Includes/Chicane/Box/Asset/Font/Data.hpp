#pragma once

#include "Chicane/Box/Essential.hpp"
#include "Chicane/Box/Asset/Font/Glyph.hpp"
#include "Chicane/Box/Asset/Font/Vendor.hpp"

namespace Chicane
{
    namespace Box
    {
        namespace Font
        {
            struct CHICANE_BOX RawData
            {
            public:
                Vendor                     vendor = Vendor::Undefined;
                std::vector<unsigned char> data   = {};
            };

            struct CHICANE_BOX ParsedData
            {
            public:
                bool hasGlyph(char inCharacter) const;
                bool hasGlyph(std::uint32_t inCode) const;

                const Glyph& getGlyph(char inCharacter) const;
                const Glyph& getGlyph(std::uint32_t inCode) const;

            public:
                std::string                              name   = "";
                std::unordered_map<std::uint32_t, Glyph> glyphs = {};
            };
        }
    }
}