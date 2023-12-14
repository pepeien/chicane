#pragma once

#include "Base.hpp"

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
				struct Frame
                {
                    void initResources(vk::Device& inLogicalDevice, vk::PhysicalDevice& inPhysicalDevice);
                    void writeDescriptorSet(vk::Device& inLogicalDevice);

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
                    Uniform::BufferObject cameraData;
                    void* cameraDataWriteLocation;
                    Vertex::Buffer cameraDataBuffer;

                    // Resources Descriptors
                    vk::DescriptorBufferInfo uniformDescriptorBufferInfo;
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