#pragma once

#include <cstdint>

#include "Chicane/Core/Math/Vec/Vec4.hpp"

#include "Chicane/Renderer.hpp"
#include "Chicane/Renderer/RHI/Image.hpp"
#include "Chicane/Renderer/RHI/LoadOp.hpp"
#include "Chicane/Renderer/RHI/StoreOp.hpp"

namespace Chicane
{
    namespace Renderer
    {
        namespace RHI
        {
            struct CHICANE_RENDERER Attachment
            {
            public:
                Image         image = {};
                LoadOp        load  = LoadOp::Load;
                StoreOp       store = StoreOp::Store;
                Vec4          clear = Vec4::sZero();
                std::uint32_t layer = 0;
            };
        }
    }
}
