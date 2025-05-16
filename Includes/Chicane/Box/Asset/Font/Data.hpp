#pragma once

#include "Chicane/Box/Essential.hpp"
#include "Chicane/Core/Math/Vec.hpp"

namespace Chicane
{
    namespace Box
    {
        namespace Font
        {
            struct CHICANE_BOX GlyphOutline
            {
            public:
                std::uint32_t code    = 0;
                Vec<2, float> advance = {};
                Vec<2, float> bearing = {};
                Curve         curve   = {};
            };

            struct CHICANE_BOX Glyph
            {
            public:
                std::uint32_t              code     = 0;
                std::uint32_t              width    = 0;
                std::uint32_t              height   = 0;
                Vec<2, float>              advance  = {};
                Vec<2, float>              bearing  = {};
                std::vector<Vec<3, float>> vertices = {};
                std::vector<std::uint32_t> indices  = {};
            };

            typedef std::unordered_map<std::uint32_t, Glyph> Glyphs;

            struct CHICANE_BOX RawData
            {
            public:
                bool hasGlyph(char inCharacter) const;
                bool hasGlyph(std::uint32_t inCode) const;

                const Glyph& getGlyph(char inCharacter) const;
                const Glyph& getGlyph(std::uint32_t inCode) const;

            public:
                std::string name   = "";
                Glyphs      glyphs = {};
            };

            struct CHICANE_BOX GlyphUsage
            {
            public:
                std::uint32_t vertexCount   = 0;
                std::uint32_t firstVertex   = 0;
                std::uint32_t firstInstance = 0;
                std::uint32_t indexCount    = 0;
                std::uint32_t firstIndex    = 0;
            };

            typedef std::unordered_map<std::uint32_t, GlyphUsage> CompiledData;
        }
    }
}