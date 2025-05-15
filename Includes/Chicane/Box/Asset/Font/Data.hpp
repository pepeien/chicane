#pragma once

#include "Chicane/Box/Essential.hpp"
#include "Chicane/Core/Math/Vec.hpp"

namespace Chicane
{
    namespace Box
    {
        namespace Font
        {
            struct CHICANE_BOX Glyph
            {
            public:
                std::uint32_t              code     = 0;             
                std::vector<Vec<2, float>> vertices = {};
                std::vector<std::uint32_t> indices  = {};
            };

            typedef std::unordered_map<std::uint32_t, Glyph> RawData;

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