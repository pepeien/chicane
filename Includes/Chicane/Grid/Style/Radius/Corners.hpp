#pragma once

#include "Chicane/Core/Math/Vec/Vec4.hpp"
#include "Chicane/Core/Size/Direction.hpp"
#include "Chicane/Core/String.hpp"

#include "Chicane/Grid.hpp"
#include "Chicane/Grid/Style/Corners.hpp"

namespace Chicane
{
    namespace Grid
    {
        struct CHICANE_GRID StyleRadiusCorners : StyleCorners
        {
        public:
            void parseWith(
                std::function<float(const String&, SizeDirection)> inParser, SizeDirection inDirection
            );

            void setOnelinerAttribute(const String& inValue);

            Vec4 values() const;

            bool isZero() const;
        };
    }
}
