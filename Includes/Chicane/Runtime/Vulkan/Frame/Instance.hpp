#pragma once

#include "Chicane/Runtime/Renderer/View/Data.hpp"
#include "Chicane/Runtime/Vulkan/Essential.hpp"
#include "Chicane/Runtime/Vulkan/Image/Data.hpp"
#include "Chicane/Runtime/Vulkan/Frame/Resource.hpp"

namespace Chicane
{
    class CCamera;
    class CLight;
    class CMesh;

    namespace Vulkan
    {
        namespace Frame
        {
            class CHICANE_RUNTIME Instance
            {
            public:
                // Lifecycle
                void wait(const vk::Device& inLogicalDevice);
                void reset(const vk::Device& inLogicalDevice);
                void destroy();

                // Sychronization
                void setupSync();
                void destroySync();

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

                // Image
                void setupColorImage(vk::Format inFormat, const vk::Extent2D& inExtent);
                void destroyColorImage();

                void setupDepthImage(vk::Format inFormat, const vk::Extent2D& inExtent);
                void destroyDepthImage();

                void setupShadowImage(vk::Format inFormat, const vk::Extent2D& inExtent);
                void destroyShadowImage();

                // Vulkan
                void addFrameBuffer(const std::string& inId, const vk::Framebuffer& inFramebuffer);
                vk::Framebuffer getFramebuffer(const std::string& inId) const;

                void addDescriptorSet(const std::string& inId, const vk::DescriptorSet& inDescriptorSet);
                vk::DescriptorSet getDescriptorSet(const std::string& inId) const;

                void addWriteDescriptorSet(const vk::WriteDescriptorSet& inWriteDescriptorSet);
                void updateDescriptorSets();

            private:
                View::Data normalizeViewData(const View::Data& outData);

                void refreshMeshData(const std::vector<CMesh*>& inMeshes);

            public:
                vk::Device                                        logicalDevice;
                vk::PhysicalDevice                                physicalDevice;

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
                Resource<View::Data>                              cameraResource;
                Resource<View::Data>                              lightResource;
                Resource<Box::Mesh::Parsed>                       meshResource;

                std::unordered_map<std::string,vk::DescriptorSet> descriptorSets;
                std::vector<vk::WriteDescriptorSet>               descriptorSetWrites;
            };
        }
    }
}