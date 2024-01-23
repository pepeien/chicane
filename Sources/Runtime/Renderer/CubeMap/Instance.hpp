#pragma once

#include "Base.hpp"

#include "Renderer/Buffer.hpp"
#include "Renderer/Descriptor.hpp"
#include "Renderer/Image.hpp"

constexpr uint32_t CUBEMAP_IMAGE_COUNT = 6;

namespace Chicane
{
    namespace CubeMap
    {
        typedef std::array<std::vector<unsigned char>, CUBEMAP_IMAGE_COUNT> Data;

        struct CreateInfo
        {
            Data data;
            vk::Device logicalDevice;
            vk::PhysicalDevice physicalDevice;
            vk::CommandBuffer commandBuffer;
            vk::Queue queue;
            vk::DescriptorSetLayout descriptorSetLayout;
            vk::DescriptorPool descriptorPool;
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
            int m_width;
            int m_height;
            int m_channels;
            Data m_data;
            stbi_uc* m_pixels[CUBEMAP_IMAGE_COUNT];
            vk::Device m_logicalDevice;
            vk::PhysicalDevice m_physicalDevice;

            Image::Bundle m_image;

            Descriptor::Bundle m_descriptor;

            vk::CommandBuffer m_commandBuffer;
            vk::Queue m_queue;
        };
    }
}