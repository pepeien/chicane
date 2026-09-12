#pragma once

#include <cstdint>
#include <memory>

#include "Chicane/Core/Image.hpp"

#include "Chicane/Renderer.hpp"
#include "Chicane/Renderer/Draw/Data.hpp"

namespace Chicane
{
    namespace Renderer
    {
        struct CHICANE_RENDERER DrawTextureData : public DrawData
        {
        public:
            Image::Instance                  image;
            std::shared_ptr<Image::MipChain> mips;
            bool                             bStreamable = true;
        };
    }
}
