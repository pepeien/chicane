#pragma once

#include "Chicane/Core/Math/Vertex.hpp"
#include "Chicane/Grid.hpp"

namespace Chicane
{
    namespace Grid
    {
        struct CHICANE_GRID Primitive
        {
        public:
            friend bool operator==(const Primitive& inLeft, const Primitive& inRight)
            {
                return (
                    inLeft.vertices == inRight.vertices &&
                    inLeft.indices  == inRight.indices
                );
            }

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
            std::vector<Vertex>        vertices = {};
            std::vector<std::uint32_t> indices  = {};
        };
    }
}