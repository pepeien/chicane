#pragma once

#include "Base.hpp"

#include "Renderer/Buffer.hpp"
#include "Renderer/Descriptor.hpp"
#include "Renderer/Image.hpp"

constexpr uint32_t TEXTURE_IMAGE_COUNT = 1;

namespace Engine
{
    namespace Texture
    {
        struct Data
        {
            std::string filepath;
        };

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
            std::string m_filepath;
            vk::Device m_logicalDevice;
            vk::PhysicalDevice m_physicalDevice;
            stbi_uc* m_pixels;

            Image::Bundle m_image;

            Descriptor::Bundle m_descriptor;

            vk::CommandBuffer m_commandBuffer;
            vk::Queue m_queue;
        };
    }
}