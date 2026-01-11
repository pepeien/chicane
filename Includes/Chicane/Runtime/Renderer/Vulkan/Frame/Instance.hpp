#pragma once

#include "Chicane/Box/Asset/Mesh/Parsed.hpp"
#include "Chicane/Runtime.hpp"
#include "Chicane/Runtime/Renderer/View.hpp"
#include "Chicane/Runtime/Renderer/Vulkan/Frame/Resource.hpp"
#include "Chicane/Runtime/Renderer/Vulkan/Image/Data.hpp"

#include <vulkan/vulkan.hpp>

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
                void addFrameBuffer(const String& inId, const vk::Framebuffer& inFramebuffer);
                vk::Framebuffer getFramebuffer(const String& inId) const;

                void addDescriptorSet(const String& inId, const vk::DescriptorSet& inDescriptorSet);
                vk::DescriptorSet getDescriptorSet(const String& inId) const;

                void addWriteDescriptorSet(const vk::WriteDescriptorSet& inWriteDescriptorSet);
                void updateDescriptorSets();

            private:
                RendererView getActiveCameraData(const std::vector<CCamera*>& inCameras);
                RendererView normalizeViewData(const RendererView& inData);

                void refreshMeshData(const std::vector<CMesh*>& inMeshes);

            public:
                vk::Device                                    logicalDevice;
                vk::PhysicalDevice                            physicalDevice;

                // Image
                Image::Data                                   colorImage;
                Image::Data                                   depthImage;
                Image::Data                                   shadowImage;
                vk::DescriptorImageInfo                       shadowImageInfo;

                // Buffer
                std::unordered_map<String, vk::Framebuffer>   framebuffers;

                // Sychronization
                vk::CommandBuffer                             commandBuffer;
                vk::Fence                                     renderFence;
                vk::Semaphore                                 presentSemaphore;
                vk::Semaphore                                 renderSemaphore;

                // Resources
                Resource<RendererView>                        cameraResource;
                Resource<RendererView>                        lightResource;
                Resource<Box::MeshParsed>                     meshResource;
                Resource<Box::MeshParsed>                     collisionResource;

                std::unordered_map<String, vk::DescriptorSet> descriptorSets;
                std::vector<vk::WriteDescriptorSet>           descriptorSetWrites;
            };
        }
    }
}