#pragma once

#include "Chicane.hpp"
#include "Chicane/Core/Math/Vec.hpp"

namespace Chicane
{
    namespace Box
    {
        namespace Model
        {
            struct Vertex
            {
            public:
                Vec<3, float> position = Vec3Zero;
                Vec<3, float> color    = Vec3Zero;
                Vec<2, float> UV       = Vec2Zero;
                Vec<3, float> normal   = Vec3Zero;
            };

            struct RawData
            {
            public:
                std::vector<Vertex>        vertices = {};
                std::vector<std::uint32_t> indices  = {};
            };

            struct CompiledData
            {
            public:
                std::uint32_t vertexCount   = 0;
                std::uint32_t firstVertex   = 0;
                std::uint32_t firstInstance = 0;
                std::uint32_t indexCount    = 0;
                std::uint32_t firstIndex    = 0;
            };
        }
    }
}