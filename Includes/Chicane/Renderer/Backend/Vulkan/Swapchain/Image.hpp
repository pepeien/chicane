#pragma once

#include <vulkan/vulkan.hpp>

#include "Chicane/Core/View.hpp"

#include "Chicane/Renderer.hpp"
#include "Chicane/Renderer/Backend/Vulkan/Frame/CreateInfo.hpp"
#include "Chicane/Renderer/Backend/Vulkan/Frame/Resource.hpp"
#include "Chicane/Renderer/Backend/Vulkan/Image/Data.hpp"
#include "Chicane/Renderer/Draw.hpp"
#include "Chicane/Renderer/Draw/Poly/2D/Instance.hpp"
#include "Chicane/Renderer/Draw/Poly/3D/Instance.hpp"
#include "Chicane/Renderer/Frame.hpp"

namespace Chicane
{
    namespace Renderer
    {
        class CHICANE_RENDERER VulkanSwapchainImage
        {
        public:
            // Lifecycle
            void init();
            void sync();
            vk::ResultValue<std::uint32_t> acquire(const vk::SwapchainKHR& inSwapchain);
            void begin(const Frame& inFrame);
            void end();
            void destroy();

            // CommandBuffer
            void setupCommandBuffer(const vk::CommandPool& inPool);

            // Buffer
            void addBuffer(const VulkanFrameCreateInfo& inCreateInfo);

            // Sychronization
            void setupSync();
            void destroySync();

            // Resources
            void setupCameraData();
            void updateCameraData(const View& inData);
            void destroyCameraData();

            void setupLightData();
            void updateLightData(const View::List& inData);
            void destroyLightData();

            void setup2DData();
            void update2DData(const DrawPoly2DInstance::List& inData);
            void destroy2DData();

            void setup3DData();
            void update3DData(const DrawPoly3DInstance::List& inData);
            void destroy3DData();

            // Image
            void setupColorImage(vk::Format inFormat, const vk::Extent2D& inExtent);
            void destroyColorImage();

            void setupDepthImage(vk::Format inFormat, const vk::Extent2D& inExtent);
            void destroyDepthImage();

            void setupShadowImage(vk::Format inFormat, const vk::Extent2D& inExtent);
            void destroyShadowImage();

            // Frambuffer
            void addFramebuffer(const String& inId, const vk::Framebuffer& inFramebuffer);
            vk::Framebuffer getFramebuffer(const String& inId) const;

            // Descriptor
            void addDescriptorSet(const String& inId, const vk::DescriptorSet& inDescriptorSet);
            vk::DescriptorSet getDescriptorSet(const String& inId) const;

            void addWriteDescriptorSet(const vk::WriteDescriptorSet& inWriteDescriptorSet);
            void updateDescriptorSets();

        public:
            vk::Device                                    logicalDevice;
            vk::PhysicalDevice                            physicalDevice;

            // Image
            VulkanImageData                               colorImage;
            VulkanImageData                               depthImage;
            VulkanImageData                               shadowImage;
            vk::DescriptorImageInfo                       shadowImageInfo;

            // Buffer
            std::unordered_map<String, vk::Framebuffer>   framebuffers;

            // Sychronization
            vk::CommandBuffer                             commandBuffer;
            vk::Semaphore                                 imageAvailableSemaphore;
            vk::Semaphore                                 renderFinishedSemaphore;
            vk::Fence                                     fence;

            // Resources
            VulkanFrameResource<View>                     cameraResource;
            VulkanFrameResource<View>                     lightResource;
            VulkanFrameResource<DrawPoly2DInstance>       poly2DResource;
            VulkanFrameResource<DrawPoly3DInstance>       poly3DResource;

            std::unordered_map<String, vk::DescriptorSet> descriptorSets;
            std::vector<vk::WriteDescriptorSet>           descriptorSetWrites;
        };
    }
}