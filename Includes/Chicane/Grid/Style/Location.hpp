#pragma once

#include <vector>

#include "Chicane/Core/String.hpp"

#include "Chicane/Grid.hpp"
#include "Chicane/Grid/Style/Location/Type.hpp"

namespace Chicane
{
    namespace Grid
    {
        struct CHICANE_GRID StyleLocation
        {
        public:
            static constexpr inline const char* URL_KEYWORD = "url";

        public:
            StyleLocation();

        public:
            void parse(const String& inValue);

        public:
            StyleLocationType type;
            String            value;
        };
    }
}