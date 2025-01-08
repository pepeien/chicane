#pragma once

#include "Chicane/Renderer/Vulkan/Base.hpp"
#include "Chicane/Renderer/Vulkan/Buffer.hpp"
#include "Chicane/Renderer/Vulkan/CubeMap/CreateInfo.hpp"
#include "Chicane/Renderer/Vulkan/Descriptor.hpp"
#include "Chicane/Renderer/Vulkan/Image/Data.hpp"

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
                void initImage();
                std::vector<std::uint32_t> getImagePixelCounts();
                void copyPixels();
                void initView();
                void initSampler();
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