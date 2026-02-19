#pragma once

#include <vulkan/vulkan.hpp>

#include "Chicane/Core/Window/Event.hpp"

#include "Chicane/Renderer.hpp"
#include "Chicane/Renderer/Draw/Texture.hpp"
#include "Chicane/Renderer/Backend.hpp"
#include "Chicane/Renderer/Instance.hpp"
#include "Chicane/Renderer/Backend/Vulkan/Descriptor/Bundle.hpp"
#include "Chicane/Renderer/Backend/Vulkan/Frame.hpp"
#include "Chicane/Renderer/Backend/Vulkan/Swapchain/Bundle.hpp"
#include "Chicane/Renderer/Backend/Vulkan/Swapchain/Image.hpp"
#include "Chicane/Renderer/Backend/Vulkan/Texture.hpp"

namespace Chicane
{
    namespace Renderer
    {
        class CHICANE_RENDERER VulkanBackend : public Backend
        {
        public:
            VulkanBackend();
            ~VulkanBackend();

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

        private:
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
            void destroySwapchain();
            void rebuildSwapchain();

            void buildFrames();
            void destroyFrames();

            void buildLayers();

            void buildTextureDescriptor();
            void buildTextureData(const DrawTexture::List& inTextures);
            void destroyTextureData();

        public:
            // Instance
            vk::Instance                                instance;

            // Devices
            vk::PhysicalDevice                          physicalDevice;
            vk::Device                                  logicalDevice;

            // Surface
            vk::SurfaceKHR                              surface;

            // Queues
            vk::Queue                                   graphicsQueue;

            // Command
            vk::CommandBuffer                           mainCommandBuffer;

            // Swap Chain
            VulkanSwapchainBundle                       swapchain;

            // Frames
            std::vector<VulkanFrame>                    frames;

            // Textures
            VulkanDescriptorBundle                      textureDescriptor;
            std::vector<std::unique_ptr<VulkanTexture>> textures;

        private:
            // Frame
            std::uint32_t                     m_currentFrameIndex;

            // Instance
            vk::detail::DispatchLoaderDynamic m_dispatcher;

            // Debug
            vk::DebugUtilsMessengerEXT        m_debugMessenger;

            // Queues
            vk::Queue                         m_presentQueue;

            // Command
            vk::CommandPool                   m_mainCommandPool;
        };
    }
}