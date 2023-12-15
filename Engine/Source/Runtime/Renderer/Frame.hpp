#pragma once

#include "Base.hpp"

#include "Model.hpp"
#include "Scene.hpp"
#include "Uniform.hpp"
#include "Vertex.hpp"

namespace Engine
{
    namespace Runtime
    {
        namespace Renderer
        {
            namespace Frame
            {
                struct CameraData
                {
                    Uniform::BufferObject object;
                    size_t allocationSize;
                    void* writeLocation;
                    Vertex::Buffer buffer;
                };

                struct ModelData
                {
                    std::vector<glm::mat4> transforms;
                    size_t allocationSize;
                    void* writeLocation;
                    Vertex::Buffer buffer;
                };

				struct Instance
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
                    CameraData cameraData;
                    ModelData modelData;

                    // Resources Descriptors
                    vk::DescriptorBufferInfo uniformDescriptorBufferInfo;
                    vk::DescriptorBufferInfo modelDescriptorBufferInfo;
                    vk::DescriptorSet descriptorSet;
                };

                struct BufferCreateInfo
                {
                    vk::Device logicalDevice;
                    vk::RenderPass renderPass;
                    vk::Extent2D swapChainExtent;
                    std::vector<Instance>& frames;
                };

				void initBuffer(BufferCreateInfo& outCreateInfo);
			}
		}
	}
}