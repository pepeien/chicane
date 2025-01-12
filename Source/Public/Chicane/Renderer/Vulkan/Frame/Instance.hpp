#pragma once

#include "Chicane/Game/Transformable/Component/CameraComponent.hpp"
#include "Chicane/Game/Transformable/Component/MeshComponent.hpp"
#include "Chicane/Renderer/Mesh/Data.hpp"
#include "Chicane/Renderer/Camera/Data.hpp"
#include "Chicane/Renderer/Vulkan/Base.hpp"
#include "Chicane/Renderer/Vulkan/Frame/Depth.hpp"
#include "Chicane/Renderer/Vulkan/Frame/Resource.hpp"

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
                // Sychronization
                void setupSync();
                void wait(const vk::Device& inLogicalDevice);
                void reset(const vk::Device& inLogicalDevice);
                void destroySync();

                vk::ResultValue<std::uint32_t> getNextIndex(
                    const vk::SwapchainKHR& inSwapchain,
                    const vk::Device& inLogicalDevice
                );

                // Resources
                bool isCameraDirty();
                void setCameraAsDirty();
                void setupCameraData(CameraComponent* inCamera);
                void updateCameraData(CameraComponent* inCamera);
                void destroyCameraData();

                bool areMeshesDirty();
                void setMeshesAsDirty();
                void setupMeshData(const std::vector<MeshComponent*>& inMeshes);
                void updateMeshData(const std::vector<MeshComponent*>& inMeshes);
                void destroyMeshData();

                void setupDepthBuffer(const vk::Format& inFormat);
                void destroyDepthBuffer();

                void addFrameBuffer(const std::string& inId, const vk::Framebuffer& inFramebuffer);
                vk::Framebuffer getFramebuffer(const std::string& inId) const;

                void addDescriptorSet(const std::string& inId, const vk::DescriptorSet& inDescriptorSet);
                vk::DescriptorSet getDescriptorSet(const std::string& inId) const;

                void addWriteDescriptorSet(const vk::WriteDescriptorSet& inWriteDescriptorSet);
                void updateDescriptorSets();

                void destroy();

            public:
                vk::Device                                        logicalDevice;
                vk::PhysicalDevice                                physicalDevice;

                // Swapchain
                std::uint32_t                                     width;
                std::uint32_t                                     height;

                // Image
                vk::Image                                         image;
                vk::ImageView                                     imageView;
                std::unordered_map<std::string, vk::Framebuffer>  framebuffers;

                // Depth
                Depth                                             depth;

                // Sychronization
                vk::CommandBuffer                                 commandBuffer;
                vk::Fence                                         renderFence;
                vk::Semaphore                                     presentSemaphore;
                vk::Semaphore                                     renderSemaphore;
        
                // Resources
                vk::DescriptorBufferInfo                          cameraDescriptorBufferInfo;
                vk::DescriptorBufferInfo                          modelDescriptorBufferInfo;

                std::unordered_map<std::string,vk::DescriptorSet> descriptorSets;
                std::vector<vk::WriteDescriptorSet>               descriptorSetWrites;

            private:
                bool                                              m_bIsCameraDirty;
                bool                                              m_bAreMeshesDirty;

                // Resources
                Resource<Chicane::Camera::Data>                   m_cameraResource;
                Resource<std::vector<Chicane::Mesh::Data>>        m_meshResource;
            };
        }
    }
}