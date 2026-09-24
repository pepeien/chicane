#pragma once

#include "Chicane/Box.hpp"

#include "Chicane/Core/FileSystem.hpp"
#include "Chicane/Core/Image.hpp"
#include "Chicane/Core/String.hpp"

namespace Chicane
{
    namespace Box
    {
        struct CHICANE_BOX ImportTexture
        {
        public:
            FileSystem::Path path;
            String           id;
            Image::Instance  image;
        };
    }
}
