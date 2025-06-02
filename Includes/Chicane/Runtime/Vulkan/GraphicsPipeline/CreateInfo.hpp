#pragma once

#include "Chicane/Runtime/Vulkan/Essential.hpp"
#include "Chicane/Runtime/Vulkan/GraphicsPipeline/Attachment.hpp"
#include "Chicane/Runtime/Vulkan/Shader/CreateInfo.hpp"

namespace Chicane
{
    namespace Vulkan
    {
        namespace GraphicsPipeline
        {
            struct CHICANE_RUNTIME CreateInfo
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
                std::vector<vk::PushConstantRange>       pushConstantRanges;

                // Vulkan
                vk::Device                               logicalDevice;
            };
        }
    }
}