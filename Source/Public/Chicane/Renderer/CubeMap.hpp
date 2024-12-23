#pragma once

#include "Chicane/Base.hpp"
#include "Chicane/Renderer/Buffer.hpp"
#include "Chicane/Renderer/Descriptor.hpp"
#include "Chicane/Renderer/Image.hpp"

constexpr uint32_t CUBEMAP_IMAGE_COUNT = 6;

namespace Chicane
{
    namespace CubeMap
    {
        typedef std::array<std::vector<unsigned char>, CUBEMAP_IMAGE_COUNT> Data;

        struct CreateInfo
        {
            Data                    data;
            vk::Device              logicalDevice;
            vk::PhysicalDevice      physicalDevice;
            vk::CommandBuffer       commandBuffer;
            vk::Queue               queue;
            vk::DescriptorSetLayout descriptorSetLayout;
            vk::DescriptorPool      descriptorPool;
        };

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
            Data               m_data;
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