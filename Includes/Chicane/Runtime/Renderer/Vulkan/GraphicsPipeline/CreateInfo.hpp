#pragma once

#include "Runtime/Renderer/Vulkan/Base.hpp"
#include "Runtime/Renderer/Vulkan/GraphicsPipeline/Attachment.hpp"
#include "Runtime/Renderer/Vulkan/Shader/CreateInfo.hpp"

namespace Chicane
{
    namespace Vulkan
    {
        namespace GraphicsPipeline
        {
            struct CHICANE CreateInfo
            {
            public:
                // Modifiers
                bool                                     bHasVertices;
                bool                                     bHasDepthWrite;
                bool                                     bHasBlending;

                // Shader
                std::vector<Shader::StageCreateInfo>     shaders;

                // Viewport
                vk::Extent2D                             extent;

                // Attachment
                std::vector<Attachment>                  attachments;

                // Rasterize
                vk::PipelineRasterizationStateCreateInfo rasterizaterizationState;

                // Pipeline Layout
                std::vector<vk::DescriptorSetLayout>     descriptorSetLayouts;

                // Vulkan
                vk::Device                               logicalDevice;
            };
        }
    }
}