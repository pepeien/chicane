#pragma once

#include "Chicane/Core/Window/Event.hpp"
#include "Chicane/Runtime/Renderer.hpp"
#include "Chicane/Runtime/Renderer/Vulkan/Data.hpp"
#include "Chicane/Runtime/Renderer/Vulkan/Frame/Instance.hpp"
#include "Chicane/Runtime/Renderer/Vulkan/Internals.hpp"
#include "Chicane/Runtime/Renderer/Vulkan/SwapChain.hpp"

#include <vulkan/vulkan.hpp>

namespace Chicane
{
    namespace Vulkan
    {
        class CHICANE_RUNTIME Renderer : public Chicane::Renderer
        {
        public:
            Renderer();
            virtual ~Renderer();

        public:
            // Lifecycle
            void onInit() override;
            void onRender() override;
            void onEvent(const WindowEvent& inEvent) override;
            void onResizing() override;
            void onRepositioning() override;

        public:
            RendererInternals getInternals();

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

            void buildSwapChain();
            void destroySwapChain();
            void rebuildSwapChain();

            void buildCommandPool();
            void destroyCommandPool();

            void buildMainCommandBuffer();
            void buildFramesCommandBuffers();

            void renderViewport(const vk::CommandBuffer& inCommandBuffer);

            void rebuildFrames();
            void setupFrame(Frame::Instance& outFrame);

            void buildLayers();
            void setupLayers();
            void renderLayers(Frame::Instance& outFrame, const vk::CommandBuffer& inCommandBuffer);

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
            SwapChain::Bundle                 m_swapChain;

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