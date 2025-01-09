#pragma once

#include "Chicane/Base.hpp"

namespace Chicane
{
    namespace Box
    {
        namespace Asset
        {
            enum class Type : std::uint8_t
            {
                Undefined,
                Mesh,    // Two entries [Model, Texture] both being refNames
                CubeMap, // 6 entries all being images [Positive X, Negative X, Positive Y, Negative Y, Positive Z, Negative Z]
                Texture, // Image
                Model,   // 3D Model,
                Audio    // .MP3 Audio
            };
        }
    }
}