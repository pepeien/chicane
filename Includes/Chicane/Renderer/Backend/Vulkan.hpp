#pragma once

#include <vulkan/vulkan.hpp>

#include "Chicane/Core/Window/Event.hpp"

#include "Chicane/Renderer.hpp"
#include "Chicane/Renderer/Backend.hpp"
#include "Chicane/Renderer/Backend/Vulkan/Data.hpp"
#include "Chicane/Renderer/Backend/Vulkan/Frame.hpp"
#include "Chicane/Renderer/Backend/Vulkan/Swapchain/Bundle.hpp"

namespace Chicane
{
    namespace Renderer
    {
        class CHICANE_RENDERER VulkanBackend : public Backend
        {
        public:
            VulkanBackend();
            virtual ~VulkanBackend();

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

            void rebuildFrames();
            void setupFrame(VulkanFrame& outFrame);

            void buildLayers();
            void setupLayers();
            void renderLayers(VulkanFrame& outFrame, const vk::CommandBuffer& inCommandBuffer);

        private:
            // Instance
            vk::Instance                      m_instance;
            vk::detail::DispatchLoaderDynamic m_dispatcher;

            // Debug
            vk::DebugUtilsMessengerEXT        m_debugMessenger;

            // Surface
            vk::SurfaceKHR                    m_surface;

            // Devices
            vk::PhysicalDevice                m_physicalDevice;
            vk::Device                        m_logicalDevice;

            // Queues
            vk::Queue                         m_graphicsQueue;
            vk::Queue                         m_presentQueue;

            // Swap Chain
            VulkanSwapchainBundle             m_swapchain;

            // Command
            vk::CommandPool                   m_mainCommandPool;
            vk::CommandBuffer                 m_mainCommandBuffer;

            // Frame
            int                               m_imageCount;
            int                               m_currentImageIndex;

            // Viewport
            vk::Viewport                      m_vkViewport;
            vk::Rect2D                        m_vkScissor;
        };
    }
}