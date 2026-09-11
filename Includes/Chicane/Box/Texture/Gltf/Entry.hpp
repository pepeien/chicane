#pragma once

#include "Chicane/Box.hpp"

#include "Chicane/Core/Image.hpp"
#include "Chicane/Core/Image/Vendor.hpp"
#include "Chicane/Core/String.hpp"

namespace Chicane
{
    namespace Box
    {
        namespace TextureGltf
        {
            struct CHICANE_BOX Entry
            {
                String      id;
                ImageVendor vendor = ImageVendor::Undefined;
                Image::Raw  data   = {};
            };
        }
    }
}
