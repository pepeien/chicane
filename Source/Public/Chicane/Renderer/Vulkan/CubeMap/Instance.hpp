#pragma once

#include "Chicane/Renderer/Vulkan/Base.hpp"
#include "Chicane/Renderer/Vulkan/Buffer.hpp"
#include "Chicane/Renderer/Vulkan/CubeMap/CreateInfo.hpp"
#include "Chicane/Renderer/Vulkan/Descriptor.hpp"
#include "Chicane/Renderer/Vulkan/Image/Bundle.hpp"

namespace Chicane
{
    namespace Vulkan
    {
        namespace CubeMap
        {
            class Instance
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
                void load();
                void populate();

                void initView();
                void initSampler();
                void initDescriptorSet();

            private:
                int                m_width;
                int                m_height;
                int                m_channel;
                CubeMap::Data      m_data;
                unsigned char*     m_pixels[CUBEMAP_IMAGE_COUNT];

                Image::Bundle      m_image;

                Descriptor::Bundle m_descriptor;

                vk::Device         m_logicalDevice;
                vk::PhysicalDevice m_physicalDevice;
                vk::CommandBuffer  m_commandBuffer;
                vk::Queue          m_queue;
            };
        }
    }
}