#pragma once

#include "Base.hpp"

#include "Renderer/Buffer.hpp"
#include "Renderer/Descriptor.hpp"
#include "Renderer/Image.hpp"
#include "Renderer/Uniform.hpp"
#include "Renderer/Sync.hpp"
#include "Renderer/Level/Instance.hpp"
#include "Renderer/Level/Actor.hpp"

namespace Chicane
{
    namespace Frame
    {
        struct CameraData
        {
            Uniform::BufferObject object;
            size_t allocationSize;
            void* writeLocation;
            Chicane::Buffer::Instance buffer;
        };
    
        struct ModelData
        {
            std::vector<glm::mat4> transforms;
            size_t allocationSize;
            void* writeLocation;
            Chicane::Buffer::Instance buffer;
        };

        struct Stats
        {
            int count;
            float time;
            float lastTime;
            float currentTime;
        };
    
        class Instance
        {
        public:
            void setupSync();
            void setupCamera();
            void setupModelData(const std::vector<Level::Actor::Instance>& inActors);
            void setupDescriptors(
                const vk::DescriptorSetLayout& inLayout,
                const vk::DescriptorPool& inPool
            );
            void setupDepthBuffering();
            void setupDescriptorSet();
            
            void updateModelTransforms(const std::vector<Level::Actor::Instance>& inActors);

            void destroy();

        public:
            vk::Device logicalDevice;
            vk::PhysicalDevice physicalDevice;

            // Swapchain
            uint32_t width;
            uint32_t height;

            vk::Image image;
            vk::ImageView imageView;
            vk::Framebuffer framebuffer;

            vk::Image depthImage;
            vk::ImageView depthImageView;
            vk::DeviceMemory depthMemory;
            vk::Format depthFormat;

            // Sychronization
            vk::CommandBuffer commandBuffer;
            vk::Fence renderFence;
            vk::Semaphore presentSemaphore;
            vk::Semaphore renderSemaphore;
    
            // Resources
            CameraData cameraData;
            ModelData modelData;
    
            // Resources Descriptors
            vk::DescriptorBufferInfo uniformDescriptorBufferInfo;
            vk::DescriptorBufferInfo modelDescriptorBufferInfo;
            vk::DescriptorSet descriptorSet;
        };
    }
}