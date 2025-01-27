#pragma once

#include "Chicane/Game/Transformable/Component/Camera.hpp"
#include "Chicane/Game/Transformable/Component/Mesh.hpp"
#include "Chicane/Game/Transformable/Component/Light.hpp"
#include "Chicane/Renderer/Mesh/Data.hpp"
#include "Chicane/Renderer/View/Data.hpp"
#include "Chicane/Renderer/Vulkan/Base.hpp"
#include "Chicane/Renderer/Vulkan/Image/Data.hpp"
#include "Chicane/Renderer/Vulkan/Frame/Resource.hpp"

namespace Chicane
{
    class CMesh;

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
                void setupCameraData(const std::vector<CCamera*>& inCameras);
                void updateCameraData(const std::vector<CCamera*>& inCameras);
                void destroyCameraData();

                void setupLightData(const std::vector<CLight*>& inLights);
                void updateLightData(const std::vector<CLight*>& inLights);
                void destroyLightData();

                void setupMeshData(const std::vector<CMesh*>& inMeshes);
                void updateMeshData(const std::vector<CMesh*>& inMeshes);
                void destroyMeshData();

                void setupColorImage(vk::Format inFormat);
                void destroyColorImage();

                void setupDepthImage(vk::Format inFormat);
                void destroyDepthImage();

                void setupShadowImage(vk::Format inFormat);
                void destroyShadowImage();

                void addFrameBuffer(const std::string& inId, const vk::Framebuffer& inFramebuffer);
                vk::Framebuffer getFramebuffer(const std::string& inId) const;

                void addDescriptorSet(const std::string& inId, const vk::DescriptorSet& inDescriptorSet);
                vk::DescriptorSet getDescriptorSet(const std::string& inId) const;

                void addWriteDescriptorSet(const vk::WriteDescriptorSet& inWriteDescriptorSet);
                void updateDescriptorSets();

                void destroy();

            private:
                void refreshMeshData(const std::vector<CMesh*>& inMeshes);

            public:
                vk::Device                                        logicalDevice;
                vk::PhysicalDevice                                physicalDevice;

                // Swapchain
                std::uint32_t                                     width;
                std::uint32_t                                     height;

                // Image
                Image::Data                                       colorImage;
                Image::Data                                       depthImage;
                Image::Data                                       shadowImage;
                vk::DescriptorImageInfo                           shadowImageInfo;

                // Buffer
                std::unordered_map<std::string, vk::Framebuffer>  framebuffers;

                // Sychronization
                vk::CommandBuffer                                 commandBuffer;
                vk::Fence                                         renderFence;
                vk::Semaphore                                     presentSemaphore;
                vk::Semaphore                                     renderSemaphore;

                // Resources
                Resource<Chicane::View::Data>                     cameraResource;
                Resource<Chicane::View::Data>                     lightResource;
                Resource<Chicane::Mesh::Data>                     meshResource;

                std::unordered_map<std::string,vk::DescriptorSet> descriptorSets;
                std::vector<vk::WriteDescriptorSet>               descriptorSetWrites;
            };
        }
    }
}