#pragma once

#include <vector>

#include "Chicane/Core/Math/Vec/Vec2.hpp"
#include "Chicane/Core/Math/Vertex.hpp"
#include "Chicane/Core/String.hpp"

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
            bool isEmpty() const;
            bool hasGlyph() const;

            void clear();

        public:
            String            reference  = String::empty();
            Vertex::List      vertices   = {};
            Vertex::Indices   indices    = {};
            String            glyph      = String::empty();
            float             dilation   = 0.0f;
            Vec2              outlineMin = Vec2::Zero();
            Vec2              outlineMax = Vec2::Zero();
            std::vector<Vec2> outline    = {};
        };
    }
}