#pragma once

#include "Chicane/Base.hpp"
#include "Chicane/Renderer/Buffer.hpp"
#include "Chicane/Renderer/CubeMap/CreateInfo.hpp"
#include "Chicane/Renderer/Descriptor.hpp"
#include "Chicane/Renderer/Image/Bundle.hpp"

namespace Chicane
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