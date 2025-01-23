#pragma once

#include "Chicane/Renderer/Vulkan/Base.hpp"
#include "Chicane/Renderer/Vulkan/GraphicsPipeline/Attachment.hpp"
#include "Chicane/Renderer/Vulkan/Shader/CreateInfo.hpp"

namespace Chicane
{
    namespace Vulkan
    {
        namespace GraphicsPipeline
        {
            struct CreateInfo
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