#pragma once

#include "Chicane/Grid/Essential.hpp"

namespace Chicane
{
    namespace Grid
    {
        struct CHICANE_GRID Primitive
        {
        public:
            struct Compiled
            {
            public:
                std::uint32_t vertexCount   = 0;
                std::uint32_t firstVertex   = 0;
                std::uint32_t indexCount    = 0;
                std::uint32_t firstIndex    = 0;
            };

        public:
            bool isEmpty() const
            {
                return vertices.empty() || indices.empty();
            }

            void clear()
            {
                vertices.clear();
                indices.clear();
            }

        public:
            std::vector<Vertex>       vertices = {};
            std::vector<std::uint32_t> indices = {};
        };
    }
}