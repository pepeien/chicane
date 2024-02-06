#pragma once

#include "Base.hpp"

#include "Game/Actor.hpp"

#include "Renderer/Buffer.hpp"
#include "Renderer/Camera.hpp"
#include "Renderer/Descriptor.hpp"
#include "Renderer/GraphicsPipeline.hpp"
#include "Renderer/Model.hpp"
#include "Renderer/Image.hpp"
#include "Renderer/Sync.hpp"

namespace Chicane
{
    namespace Frame
    {    
        class Instance
        {
        public:
            void setupSync();

            void setupCameraVectorUBO();
            void setupCameraMatrixUBO();
            void setupModelData(std::vector<Actor*> inActors);
            void updateModelData(std::vector<Actor*> inActors);
    
            void setupDepthBuffering();

            void addFrameBuffer(const std::string& inId, const vk::Framebuffer& inFramebuffer);
            vk::Framebuffer getFramebuffer(const std::string& inId);

            void addDescriptorSet(const std::string& inId, const vk::DescriptorSet& inDescriptorSet);
            vk::DescriptorSet getDescriptorSet(const std::string& inId);

            void addWriteDescriptorSet(const vk::WriteDescriptorSet& inWriteDescriptorSet);
            void updateDescriptorSets();

            void destroy();

        public:
            vk::Device logicalDevice;
            vk::PhysicalDevice physicalDevice;

            // Swapchain
            uint32_t width;
            uint32_t height;

            vk::Image image;
            vk::ImageView imageView;
            std::unordered_map<std::string, vk::Framebuffer> framebuffers;

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
            MatrixUBOBundle cameraMatrixUBO;
            vk::DescriptorBufferInfo cameraMatrixDescriptorBufferInfo;

            VectorUBOBundle cameraVectorUBO;
            vk::DescriptorBufferInfo cameraVectorDescriptorBufferInfo;

            Model::Bundle modelData;
            vk::DescriptorBufferInfo modelDescriptorBufferInfo;

            std::unordered_map<std::string,vk::DescriptorSet> descriptorSets;
            std::vector<vk::WriteDescriptorSet> descriptorSetWrites;
        };
    }
}