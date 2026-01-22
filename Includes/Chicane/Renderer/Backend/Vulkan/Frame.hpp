#pragma once

#include <vulkan/vulkan.hpp>

#include "Chicane/Core/View.hpp"

#include "Chicane/Renderer.hpp"
#include "Chicane/Renderer/Backend/Vulkan/Frame/CreateInfo.hpp"
#include "Chicane/Renderer/Backend/Vulkan/Frame/Resource.hpp"
#include "Chicane/Renderer/Backend/Vulkan/Image/Data.hpp"
#include "Chicane/Renderer/Draw.hpp"
#include "Chicane/Renderer/Draw/Poly/3D/Instance.hpp"

namespace Chicane
{
    namespace Renderer
    {
        class CHICANE_RENDERER VulkanFrame
        {
        public:
            // Lifecycle
            void wait(const vk::Device& inLogicalDevice);
            void reset(const vk::Device& inLogicalDevice);
            void destroy();

            // Buffer
            void addBuffer(const VulkanFrameCreateInfo& inCreateInfo);

            // Sychronization
            void setupSync();
            void destroySync();

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
            vk::Fence                                     renderFence;
            vk::Semaphore                                 presentSemaphore;
            vk::Semaphore                                 renderSemaphore;

            // Resources
            VulkanFrameResource<View>                     cameraResource;
            VulkanFrameResource<View>                     lightResource;
            VulkanFrameResource<DrawPoly3DInstance>       meshResource;

            std::unordered_map<String, vk::DescriptorSet> descriptorSets;
            std::vector<vk::WriteDescriptorSet>           descriptorSetWrites;
        };
    }
}