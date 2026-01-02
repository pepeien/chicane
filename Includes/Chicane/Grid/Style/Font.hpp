#pragma once

#include "Chicane/Grid/Essential.hpp"

namespace Chicane
{
    namespace Grid
    {
        struct CHICANE_GRID StyleFont
        {
        public:
            static constexpr const float BASE_SIZE = 16.0f;

        public:
            StyleFont();

        public:
            String family;
            float  size;
        };
    }
}