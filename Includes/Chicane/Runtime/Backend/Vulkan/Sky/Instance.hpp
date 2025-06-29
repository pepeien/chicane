#pragma once

#include "Chicane/Runtime/Backend/Vulkan/Essential.hpp"
#include "Chicane/Runtime/Backend/Vulkan/Buffer.hpp"
#include "Chicane/Runtime/Backend/Vulkan/Sky/CreateInfo.hpp"
#include "Chicane/Runtime/Backend/Vulkan/Descriptor.hpp"
#include "Chicane/Runtime/Backend/Vulkan/Image/Data.hpp"

namespace Chicane
{
    namespace Vulkan
    {
        namespace Sky
        {
            class CHICANE_RUNTIME Instance
            {
            public:
                Instance(const CreateInfo& inCreateInfo);
                ~Instance();

            public:
                void bind(
                    const vk::CommandBuffer& inCommandBuffer,
                    const vk::PipelineLayout& inPipelineLayout
                );

            private:
                void initImage();
                void copyPixels();
                void initDescriptorSet();

            private:
                Image::Data        m_image;
                Images             m_images;

                Descriptor::Bundle m_descriptor;

                vk::Device         m_logicalDevice;
                vk::PhysicalDevice m_physicalDevice;
                vk::CommandBuffer  m_commandBuffer;
                vk::Queue          m_queue;
            };
        }
    }
}