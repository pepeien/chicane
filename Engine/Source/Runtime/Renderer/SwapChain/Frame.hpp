#pragma once

#include "Base.hpp"

#include "Renderer/Model.hpp"
#include "Renderer/Scene.hpp"
#include "Renderer/Uniform.hpp"
#include "Renderer/Vertex.hpp"

namespace Engine
{
    namespace Runtime
    {
        namespace Renderer
        {
            namespace SwapChain
            {
                struct FrameCameraData
                {
                    Uniform::BufferObject object;
                    size_t allocationSize;
                    void* writeLocation;
                    Vertex::Buffer buffer;
                };

                struct FrameModelData
                {
                    std::vector<glm::mat4> transforms;
                    size_t allocationSize;
                    void* writeLocation;
                    Vertex::Buffer buffer;
                };

				struct Frame
                {
                public:
                    void initResources(vk::Device& inLogicalDevice, vk::PhysicalDevice& inPhysicalDevice, Scene::Instance& inScene);
                    void writeDescriptorSet(vk::Device& inLogicalDevice);

                private:
                    void createCameraData(vk::Device& inLogicalDevice, vk::PhysicalDevice& inPhysicalDevice);
                    void createModelData(vk::Device& inLogicalDevice, vk::PhysicalDevice& inPhysicalDevice, Scene::Instance& inScene);

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
                    FrameCameraData cameraData;
                    FrameModelData modelData;

                    // Resources Descriptors
                    vk::DescriptorBufferInfo uniformDescriptorBufferInfo;
                    vk::DescriptorBufferInfo modelDescriptorBufferInfo;
                    vk::DescriptorSet descriptorSet;
                };

                struct FramebufferCreateInfo
                {
                    vk::Device logicalDevice;
                    vk::RenderPass renderPass;
                    vk::Extent2D swapChainExtent;
                    std::vector<Frame>& frames;
                };

				void initFramebuffers(FramebufferCreateInfo& outCreateInfo);
			}
		}
	}
}