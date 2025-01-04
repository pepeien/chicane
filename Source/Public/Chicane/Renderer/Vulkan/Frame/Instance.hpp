#pragma once

#include "Chicane/Renderer/Vulkan/Base.hpp"
#include "Chicane/Renderer/Vulkan/Camera/UBO.hpp"
#include "Chicane/Renderer/Vulkan/Frame/Depth.hpp"
#include "Chicane/Renderer/Vulkan/Mesh.hpp"

namespace Chicane
{
    class MeshComponent;

    namespace Vulkan
    {
        namespace Frame
        {
            class Instance
            {
            public:
                bool isDirty();
                void setAsDirty();

                void setupSync();
                void wait(const vk::Device& inLogicalDevice);
                void reset(const vk::Device& inLogicalDevice);
                void destroySync();

                vk::ResultValue<std::uint32_t> getNextIndex(
                    const vk::SwapchainKHR& inSwapchain,
                    const vk::Device& inLogicalDevice
                );

                void setupCameraUBO();
                void destroyCameraUBO();

                void setupMeshData(const std::vector<MeshComponent*>& inMeshes);
                void updateMeshData(const std::vector<MeshComponent*>& inMeshes);
                void destroyMeshData();
        
                void setupDepthBuffering(const vk::Format& inFormat);
                void destroyDepthBuffering();

                void addFrameBuffer(const std::string& inId, const vk::Framebuffer& inFramebuffer);
                vk::Framebuffer getFramebuffer(const std::string& inId) const;

                void addDescriptorSet(const std::string& inId, const vk::DescriptorSet& inDescriptorSet);
                vk::DescriptorSet getDescriptorSet(const std::string& inId) const;

                void addWriteDescriptorSet(const vk::WriteDescriptorSet& inWriteDescriptorSet);
                void updateDescriptorSets();

                void destroy();

            public:
                vk::Device logicalDevice;
                vk::PhysicalDevice physicalDevice;

                // Swapchain
                std::uint32_t width;
                std::uint32_t height;

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

                Mesh::Bundle meshBundle;
                vk::DescriptorBufferInfo modelDescriptorBufferInfo;

                std::unordered_map<std::string,vk::DescriptorSet> descriptorSets;
                std::vector<vk::WriteDescriptorSet> descriptorSetWrites;

            private:
                bool m_bIsDirty;
            };
        }
    }
}