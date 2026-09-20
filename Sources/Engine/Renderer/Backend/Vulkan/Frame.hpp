#pragma once

#include <cstddef>

#include <vulkan/vulkan.hpp>

#include "Chicane/Core/View.hpp"

#include "Chicane/Renderer.hpp"
#include "Backend/Vulkan/Frame/Buffer/CreateInfo.hpp"
#include "Backend/Vulkan/Frame/Resource.hpp"
#include "Backend/Vulkan/Swapchain/Image.hpp"
#include "Chicane/Renderer/Draw.hpp"
#include "Chicane/Renderer/Draw/Particle.hpp"
#include "Chicane/Renderer/Draw/Poly/2D/Instance.hpp"
#include "Chicane/Renderer/Draw/Poly/3D/Instance.hpp"
#include "Chicane/Renderer/Frame.hpp"
#include "Chicane/Renderer/Shadow.hpp"

namespace Chicane
{
    namespace Renderer
    {
        class VulkanAllocator;

        class CHICANE_RENDERER VulkanFrame
        {
        public:
            using DescriptorSets      = std::unordered_map<String, vk::DescriptorSet>;
            using WriteDescriptorSets = std::vector<vk::WriteDescriptorSet>;

        public:
            // Lifecycle
            void wait();
            void reset();
            void begin(const Frame& inFrame, const VulkanSwapchainImage& inImage);
            void flushTarget(bool bInShouldPresentToWindow);
            void end();
            void destroy();

            // CommandBuffer
            void setupCommandBuffer(const vk::CommandPool& inPool);

            // Sychronization
            void setupSync();
            void destroySync();

            // Resources
            void setupCameraData();
            void updateCameraData(const View& inData);
            void destroyCameraData();

            void setupLightData();
            void updateLightData(const Frame& inFrame);
            void destroyLightData();

            void setup2DData(std::size_t inBudget);
            void update2DData(const DrawPoly2DInstance::List& inData);
            void destroy2DData();

            void setup3DData(std::size_t inBudget);
            void update3DData(const DrawPoly3DInstance::List& inData);
            void destroy3DData();

            void setupParticleData(std::size_t inBudget);
            void updateParticleData(const DrawParticle::List& inData);
            void destroyParticleData();

            // Descriptor
            void addDescriptorSet(const String& inId, const vk::DescriptorSet& inDescriptorSet);
            vk::DescriptorSet getDescriptorSet(const String& inId) const;

            void addWriteDescriptorSet(const vk::WriteDescriptorSet& inWriteDescriptorSet);
            void updateDescriptorSets();

        public:
            // Device
            vk::Device                              logicalDevice;
            vk::PhysicalDevice                      physicalDevice;
            VulkanAllocator*                        allocator = nullptr;

            // Sychronization
            vk::CommandBuffer                       commandBuffer;
            vk::Semaphore                           imageAvailableSemaphore;
            vk::Fence                               fence;

            // Resource
            VulkanFrameResource<View>               cameraResource;
            VulkanFrameResource<ShadowLight>        lightResource;
            ShadowLight                             lightData = {};
            VulkanFrameResource<DrawPoly2DInstance> poly2DResource;
            VulkanFrameResource<DrawPoly3DInstance> poly3DResource;
            VulkanFrameResource<DrawParticle>       particleResource;

            // Descriptor
            DescriptorSets                          descriptorSets;
            WriteDescriptorSets                     descriptorSetWrites;

            // Swapchain
            VulkanSwapchainImage                    image = {};
        };
    }
}