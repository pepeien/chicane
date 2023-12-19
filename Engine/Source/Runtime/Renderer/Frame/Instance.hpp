#pragma once

#include "Base.hpp"

#include "Renderer/Uniform.hpp"
#include "Renderer/Scene/Instance.hpp"
#include "Renderer/Vertex/Buffer.hpp"

namespace Engine
{
    namespace Frame
    {
        struct CameraData
        {
            Uniform::BufferObject object;
            size_t allocationSize;
            void* writeLocation;
            Vertex::Buffer::Instance buffer;
        };
    
        struct ModelData
        {
            std::vector<glm::mat4> transforms;
            size_t allocationSize;
            void* writeLocation;
            Vertex::Buffer::Instance buffer;
        };
    
        struct Instance
        {
        public:
            void initResources(
                const vk::Device& inLogicalDevice,
                const vk::PhysicalDevice& inPhysicalDevice,
                const Scene::Instance& inScene
            );
            void writeDescriptorSet(const vk::Device& inLogicalDevice);
    
        private:
            void createCameraData(const vk::Device& inLogicalDevice, const vk::PhysicalDevice& inPhysicalDevice);
            void createModelData(
                const vk::Device& inLogicalDevice,
                const vk::PhysicalDevice& inPhysicalDevice,
                const Scene::Instance& inScene
            );
    
        public:
            // Swapchain
            vk::Image image;
            vk::ImageView imageView;
            vk::Framebuffer framebuffer;
    
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