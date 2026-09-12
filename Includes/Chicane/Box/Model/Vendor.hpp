#pragma once

#include "Chicane/Box.hpp"

namespace Chicane
{
    namespace Box
    {
        enum class ModelVendor : std::uint8_t
        {
            Undefined, // N/A
            Wavefront, // .obj
            Gltf       // .gltf / .glb
        };

        inline String toString(ModelVendor inValue)
        {
            switch (inValue)
            {
            case ModelVendor::Undefined:
                return "Undefined";

            case ModelVendor::Wavefront:
                return "Wavefront";

            case ModelVendor::Gltf:
                return "Gltf";

            default:
                return "";
            }
        }
    }
}