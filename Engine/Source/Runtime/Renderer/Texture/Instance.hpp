#pragma once

#include "Base.hpp"

#include "Renderer/Buffer.hpp"
#include "Renderer/Descriptor.hpp"
#include "Renderer/Image.hpp"

namespace Chicane
{
    namespace Texture
    {
        struct Data
        {
            uint32_t width;
            uint32_t height;
            std::string filename;
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
            int width;
            int height;
            int channels;
            std::string filename;
            vk::Device logicalDevice;
            vk::PhysicalDevice physicalDevice;
            stbi_uc* pixels;

            Image::Bundle image;

            Descriptor::Bundle descriptor;

            vk::CommandBuffer commandBuffer;
            vk::Queue queue;
        };
    }
}