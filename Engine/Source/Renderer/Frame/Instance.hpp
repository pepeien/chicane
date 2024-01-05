#pragma once

#include "Base.hpp"

#include "Renderer/Buffer.hpp"
#include "Renderer/Camera.hpp"
#include "Renderer/Descriptor.hpp"
#include "Renderer/GraphicsPipeline.hpp"
#include "Renderer/Model.hpp"
#include "Renderer/Image.hpp"
#include "Renderer/Sync.hpp"
#include "Renderer/Level/Instance.hpp"
#include "Renderer/Level/Actor/Pawn.hpp"

namespace Chicane
{
    namespace Frame
    {
        struct Stats
        {
            int count            = 0;
            float time           = 0.0f;
            uint64_t lastTime    = 0;
            uint64_t currentTime = 0;
        };
    
        class Instance
        {
        public:
            void setupSync();
            void setupCameraVectorUBO();
            void setupCameraMatrixUBO();
            void setupModelData(
                const std::vector<Level::Actor::Pawn>& inActors
            );
            void setupDescriptorSet(
                GraphicsPipeline::Type inType,
                const vk::DescriptorSetLayout& inLayout,
                const vk::DescriptorPool& inPool
            );
            void setupDepthBuffering();
            void setupDescriptorSet();
            
            void updateModelTransforms(
                const std::vector<Level::Actor::Pawn>& inActors
            );

            void destroy();

        public:
            vk::Device logicalDevice;
            vk::PhysicalDevice physicalDevice;

            // Swapchain
            uint32_t width;
            uint32_t height;

            vk::Image image;
            vk::ImageView imageView;
            std::unordered_map<GraphicsPipeline::Type, vk::Framebuffer> framebuffers;

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
            Camera::MatrixUBOBundle cameraMatrixUBO;
            vk::DescriptorBufferInfo cameraMatrixDescriptorBufferInfo;

            Camera::VectorUBOBundle cameraVectorUBO;
            vk::DescriptorBufferInfo cameraVectorDescriptorBufferInfo;

            Model::Bundle modelData;
            vk::DescriptorBufferInfo modelDescriptorBufferInfo;

            std::unordered_map<GraphicsPipeline::Type, vk::DescriptorSet> descriptorSets;
        };
    }
}