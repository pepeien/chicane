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
                Audio,   // .MP3 Audio
                Mesh,    // Two entries [Model, Texture] both being refNames
                Model,   // 3D Model
                CubeMap, // 6 entries all being images [Positive X, Negative X, Positive Y, Negative Y, Positive Z, Negative Z]
                Texture, // Image
            };
        }
    }
}