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
            friend inline bool operator==(const Primitive& inLeft, const Primitive& inRight)
            {
                return (inLeft.vertices == inRight.vertices && inLeft.indices == inRight.indices);
            }

        public:
            inline bool isEmpty() const { return vertices.empty() || indices.empty(); }

            inline void clear()
            {
                scale = Vec2::Zero;
                vertices.clear();
                indices.clear();
            }

        public:
            Vec2            scale    = Vec2::Zero;
            Vertex::List    vertices = {};
            Vertex::Indices indices  = {};
        };
    }
}