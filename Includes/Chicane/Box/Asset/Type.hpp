#pragma once

#include "Chicane/Box/Essential.hpp"

namespace Chicane
{
    namespace Box
    {
        namespace Asset
        {
            enum class Type : std::uint8_t
            {
                Undefined, // N/A
                Audio,     // Audio
                Font,      // Text Font
                Mesh,      // Two entries [Model, Texture] both being refNames
                Model,     // 3D Model
                Sky,       // 6 entries all being images [Positive X, Negative X, Positive Y, Negative Y, Positive Z, Negative Z]
                Texture,   // Image
            };
        }
    }
}