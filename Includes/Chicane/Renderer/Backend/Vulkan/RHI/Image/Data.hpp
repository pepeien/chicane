#pragma once

#include <cstdint>

#include "Chicane/Renderer.hpp"
#include "Chicane/Renderer/Backend/Vulkan/Image/Info.hpp"
#include "Chicane/Renderer/RHI/Image/Format.hpp"
#include "Chicane/Renderer/RHI/Image/Kind.hpp"

namespace Chicane
{
    namespace Renderer
    {
        struct CHICANE_RENDERER VulkanRHIImageData
        {
        public:
            VulkanImageInfo     info       = {};
            RHI::ImageKind      kind       = RHI::ImageKind::Color2D;
            RHI::ImageFormat    format     = RHI::ImageFormat::RGBA8;
            std::uint32_t       layers     = 1;
            std::uint32_t       mips       = 1;
            vk::ImageLayout     layout     = vk::ImageLayout::eUndefined;
            bool                bOwned     = true;
            bool                bViewOnly  = false;
            bool                bIsSampled = false;
            bool                bHasColor  = false;
            bool                bHasDepth  = false;
            bool                bPresent   = false;
            VulkanRHIImageData* parent     = nullptr;
        };
    }
}
