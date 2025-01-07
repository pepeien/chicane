#pragma once

#include "Chicane/Base.hpp"
#include "Chicane/Core.hpp"

namespace Chicane
{
    namespace Texture
    {
        struct Data
        {
        public:
            int           width;
            int           height;
            int           channels;
            Image::Pixels pixels;
        };
    }
}