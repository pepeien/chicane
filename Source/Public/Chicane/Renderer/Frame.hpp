#pragma once

#include "Chicane/Base.hpp"
#include "Chicane/Game/Actor.hpp"
#include "Chicane/Renderer/Buffer.hpp"
#include "Chicane/Renderer/Camera.hpp"
#include "Chicane/Renderer/Descriptor.hpp"
#include "Chicane/Renderer/GraphicsPipeline.hpp"
#include "Chicane/Renderer/Model.hpp"
#include "Chicane/Renderer/Image.hpp"
#include "Chicane/Renderer/Sync.hpp"

namespace Chicane
{
    namespace Frame
    {
        struct Depth
        {
            vk::Image image;
            vk::ImageView imageView;
            vk::DeviceMemory memory;
            vk::Format format;
        };

        class Instance
        {
        public:
            void setupSync();

            void setupCameraUBO();

            void setupModelData(const std::vector<Actor*>& inActors);
            void updateModelData(const std::vector<Actor*>& inActors);
    
            void setupDepthBuffering(const vk::Format& inFormat);

            void addFrameBuffer(const std::string& inId, const vk::Framebuffer& inFramebuffer);
            vk::Framebuffer getFramebuffer(const std::string& inId);

            void addDescriptorSet(const std::string& inId, const vk::DescriptorSet& inDescriptorSet);
            vk::DescriptorSet getDescriptorSet(const std::string& inId);

            void addWriteDescriptorSet(const vk::WriteDescriptorSet& inWriteDescriptorSet);
            void updateDescriptorSets();

            void destroyCameraMemory();
            void destroyModelMemory();
            void destroy();

        public:
            vk::Device logicalDevice;
            vk::PhysicalDevice physicalDevice;

            // Swapchain
            uint32_t width;
            uint32_t height;

            // Image
            vk::Image image;
            vk::ImageView imageView;
            std::unordered_map<std::string, vk::Framebuffer> framebuffers;

            // Depth
            Depth depth;

            // Sychronization
            vk::CommandBuffer commandBuffer;
            vk::Fence renderFence;
            vk::Semaphore presentSemaphore;
            vk::Semaphore renderSemaphore;
    
            // Resources
            Camera::UBOBundle cameraUBO;
            vk::DescriptorBufferInfo cameraDescriptorBufferInfo;

            Model::Bundle modelData;
            vk::DescriptorBufferInfo modelDescriptorBufferInfo;

            std::unordered_map<std::string,vk::DescriptorSet> descriptorSets;
            std::vector<vk::WriteDescriptorSet> descriptorSetWrites;
        };
    }
}