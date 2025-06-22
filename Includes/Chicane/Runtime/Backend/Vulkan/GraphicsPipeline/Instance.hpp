#pragma once

#include "Chicane/Runtime/Backend/Vulkan/Essential.hpp"
#include "Chicane/Runtime/Backend/Vulkan/GraphicsPipeline/CreateInfo.hpp"

namespace Chicane
{
    namespace Vulkan
    {
        namespace GraphicsPipeline
        {
            class CHICANE_RUNTIME Instance
            {
            public:
                Instance(const CreateInfo& inCreateInfo);
                ~Instance();

            public:
                void bind(vk::CommandBuffer& inCommandBuffer);
                void bindDescriptorSet(
                    vk::CommandBuffer& inCommandBuffer,
                    std::uint32_t inIndex,
                    vk::DescriptorSet inDescriptorSet
                );

            public:
                vk::PipelineLayout layout;
                vk::RenderPass     renderPass;
                vk::Pipeline       instance;

            private:
                vk::Device         m_logicalDevice;
            };
        }
    }
}