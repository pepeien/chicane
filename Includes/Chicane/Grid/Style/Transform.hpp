#pragma once

#include "Chicane/Core/Math/Vec/Vec2.hpp"

#include "Chicane/Grid.hpp"

namespace Chicane
{
    namespace Grid
    {
        struct CHICANE_GRID StyleTransform
        {
        public:
            StyleTransform();

        public:
            bool isIdentity() const;

        public:
            Vec2  translation;
            float rotation;
            Vec2  scale;
        };

        inline bool operator==(const StyleTransform& inLeft, const StyleTransform& inRight)
        {
            return inLeft.translation == inRight.translation && inLeft.rotation == inRight.rotation &&
                   inLeft.scale == inRight.scale;
        }
    }
}
