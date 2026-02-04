#pragma once

#include <vulkan/vulkan.hpp>

#include "Chicane/Core/Window/Event.hpp"

#include "Chicane/Renderer.hpp"
#include "Chicane/Renderer/Draw/Texture.hpp"
#include "Chicane/Renderer/Backend.hpp"
#include "Chicane/Renderer/Backend/Vulkan/Data.hpp"
#include "Chicane/Renderer/Backend/Vulkan/Descriptor/Bundle.hpp"
#include "Chicane/Renderer/Backend/Vulkan/Frame.hpp"
#include "Chicane/Renderer/Backend/Vulkan/Swapchain/Bundle.hpp"
#include "Chicane/Renderer/Backend/Vulkan/Texture.hpp"

namespace Chicane
{
    namespace Renderer
    {
        class CHICANE_RENDERER VulkanBackend : public Backend<Frame>
        {
        public:
            VulkanBackend();
            virtual ~VulkanBackend();

        protected:
            void onInit() override;
            void onResize(const Vec<2, std::uint32_t>& inResolution) override;
            void onLoad(const DrawTexture::List& inResources) override;
            void onRender() override;

            void onHandle(const WindowEvent& inEvent) override;

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

            void buildSwapchain();
            void destroySwapchain();
            void rebuildSwapchain();

            void buildCommandPool();
            void destroyCommandPool();

            void buildMainCommandBuffer();
            void buildFramesCommandBuffers();

            void renderViewport(const vk::CommandBuffer& inCommandBuffer);

            void buildLayers();
            void renderLayers(void* inData);

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

            // Frame
            int                                         imageCount;

            // Viewport
            vk::Viewport                                viewport;
            vk::Rect2D                                  scissor;

            // Textures
            VulkanDescriptorBundle                      textureDescriptor;
            std::vector<std::unique_ptr<VulkanTexture>> textures;

        private:
            // Instance
            vk::detail::DispatchLoaderDynamic m_dispatcher;

            // Debug
            vk::DebugUtilsMessengerEXT        m_debugMessenger;

            // Queues
            vk::Queue                         m_presentQueue;

            // Command
            vk::CommandPool                   m_mainCommandPool;

            // Frame
            int                               m_currentImageIndex;
        };
    }
}