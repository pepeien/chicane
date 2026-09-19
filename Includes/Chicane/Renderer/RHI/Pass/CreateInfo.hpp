#pragma once

#include <cstdint>

#include "Chicane/Renderer.hpp"
#include "Chicane/Renderer/RHI/Attachment.hpp"

namespace Chicane
{
    namespace Renderer
    {
        namespace RHI
        {
            struct CHICANE_RENDERER PassCreateInfo
            {
            public:
                bool          bHasColor = false;
                Attachment    color     = {};

                bool          bHasDepth = false;
                Attachment    depth     = {};

                std::uint32_t width  = 0;
                std::uint32_t height = 0;
            };
        }
    }
}
