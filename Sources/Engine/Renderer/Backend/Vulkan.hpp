#pragma once

#include <vulkan/vulkan.hpp>

#include "Chicane/Core/Window/Event.hpp"

#include "Chicane/Renderer.hpp"
#include "Chicane/Renderer/Draw.hpp"
#include "Chicane/Renderer/Draw/Texture.hpp"
#include "Chicane/Renderer/Backend.hpp"
#include "Chicane/Renderer/Instance.hpp"
#include "Backend/Vulkan/Descriptor/Bundle.hpp"
#include "Backend/Vulkan/Frame.hpp"
#include "Backend/Vulkan/Allocator.hpp"
#include "Backend/Vulkan/Image/Info.hpp"
#include "Backend/Vulkan/Swapchain/Bundle.hpp"
#include "Backend/Vulkan/Swapchain/Image.hpp"
#include "Backend/Vulkan/Texture.hpp"
#include "Chicane/Renderer/RHI/Frame.hpp"
#include "Chicane/Renderer/RHI/FullscreenPass.hpp"

namespace Chicane
{
    namespace Renderer
    {
        class CHICANE_RENDERER VulkanBackend : public Backend
        {
        public:
            VulkanBackend();
            ~VulkanBackend();

        public:
            Draw::Id getScreenTextureId() const override;
            bool captureScreen(
                std::uint32_t& outWidth, std::uint32_t& outHeight, std::vector<unsigned char>& outRgba
            ) override;

        protected:
            // Lifecycle
            void onInit() override;
            void onShutdown() override;

            // Event
            void onResize() override;
            void onLoad(const DrawTextureResource& inResources) override;

            // Render
            void onRender(const Frame& inFrame) override;

        public:
            vk::Viewport getVkViewport(Layer* inLayer) const;
            vk::Rect2D getVkScissor(Layer* inLayer) const;
            RHI::Viewport getRHIViewport(Layer* inLayer) const override;
            RHI::Scissor getRHIScissor(Layer* inLayer) const override;
            vk::DescriptorSet getTextureDescriptorSet() const;
            vk::Format getSceneColorFormat() const;
            void releaseBoundDescriptors();

            RHI::FullscreenPass bloom;

        private:
            void fillRhiFrame(VulkanFrame& inFrame, const VulkanSwapchainImage& inImage);
            void releaseRhiWraps();

            void buildInstance();
            void destroyInstance();

            void buildDebugMessenger();
            void destroyDebugMessenger();

            void buildSurface();
            void destroySurface();

            void buildQueues();

            void buildDevices();
            void destroyDevices();

            void updateResourceBudget();

            void buildCommandPool();
            void destroyCommandPool();

            void buildMainCommandBuffer();

            void buildSwapchain();
            void setupSwapchainImages();
            void destroySwapchain();
            void rebuildSwapchain();
            vk::Extent2D getSwapchainFallbackExtent() const;

            void buildFrames();
            void destroyFrames();

            void buildTimestampQueries();
            void destroyTimestampQueries();
            void resolveGpuTimestamp(std::uint32_t inFrameIndex);
            void writeGpuTimestampStart(const vk::CommandBuffer& inCommandBuffer, std::uint32_t inFrameIndex);
            void writeGpuTimestampEnd(const vk::CommandBuffer& inCommandBuffer, std::uint32_t inFrameIndex);

            void buildLayers();

            void buildTextureDescriptor();
            void buildTextureData(const DrawTexture::List& inTextures);
            void writeTextureDescriptor(Draw::Id inId, const vk::DescriptorImageInfo& inInfo);
            bool needsTextureUpload(const DrawTexture& inTexture) const;
            void destroyTextureData();
            void bindScreenTarget(const VulkanImageInfo& inTarget);

        public:
            // Instance
            vk::Instance instance;

            // Devices
            vk::PhysicalDevice physicalDevice;
            vk::Device         logicalDevice;
            VulkanAllocator    allocator;

            // Surface
            vk::SurfaceKHR surface;

            // Queues
            vk::Queue graphicsQueue;

            // Command
            vk::CommandBuffer mainCommandBuffer;

            // Swap Chain
            VulkanSwapchainBundle swapchain;

            // Frames
            std::vector<VulkanFrame> frames;

            // Textures
            VulkanDescriptorBundle                      textureDescriptor;
            std::vector<vk::DescriptorSet>              textureDescriptorSets;
            std::vector<std::shared_ptr<VulkanTexture>> textures;

        private:
            // Frame
            std::uint32_t m_currentFrameIndex;
            std::uint32_t m_lastImageIndex;
            Draw::Id      m_screenTextureId;

            // Instance
            vk::DispatchLoaderDynamic m_dispatcher;

            // Debug
            vk::DebugUtilsMessengerEXT m_debugMessenger;

            // Queues
            vk::Queue m_presentQueue;

            // Command
            vk::CommandPool m_mainCommandPool;

            // GPU timing
            vk::QueryPool     m_timestampQueryPool;
            float             m_timestampPeriod;
            bool              m_bIsTimestampsEnabled;
            std::vector<bool> m_timestampSubmitted;
            RHI::Frame        m_rhiFrame;
            RHI::Sampler      m_linearSampler;
        };
    }
}