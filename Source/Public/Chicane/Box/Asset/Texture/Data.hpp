#pragma once

#include "Chicane/Base.hpp"
#include "Chicane/Core.hpp"

namespace Chicane
{
    namespace Box
    {
        namespace Texture
        {
            struct CompiledData
            {
            public:
                int           width;
                int           height;
                int           channels;
                Image::Pixels pixels;
            };
        }
    }
}