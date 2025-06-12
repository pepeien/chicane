#pragma once

#include "Chicane/Runtime/Renderer.hpp"
#include "Chicane/Runtime/Vulkan/Essential.hpp"
#include "Chicane/Runtime/Vulkan/Buffer.hpp"
#include "Chicane/Runtime/Vulkan/CommandBuffer.hpp"
#include "Chicane/Runtime/Vulkan/Sky.hpp"
#include "Chicane/Runtime/Vulkan/Debug.hpp"
#include "Chicane/Runtime/Vulkan/Descriptor.hpp"
#include "Chicane/Runtime/Vulkan/Device.hpp"
#include "Chicane/Runtime/Vulkan/Frame.hpp"
#include "Chicane/Runtime/Vulkan/GraphicsPipeline.hpp"
#include "Chicane/Runtime/Vulkan/Image.hpp"
#include "Chicane/Runtime/Vulkan/Instance.hpp"
#include "Chicane/Runtime/Vulkan/Queue.hpp"
#include "Chicane/Runtime/Vulkan/Shader.hpp"
#include "Chicane/Runtime/Vulkan/Surface.hpp"
#include "Chicane/Runtime/Vulkan/SwapChain.hpp"
#include "Chicane/Runtime/Vulkan/Sync.hpp"
#include "Chicane/Runtime/Vulkan/Texture.hpp"
#include "Chicane/Runtime/Vulkan/Vertex.hpp"

namespace Chicane
{
    namespace Vulkan
    {
        class CHICANE_RUNTIME Renderer : public Chicane::Renderer::Instance
        {
        public:
            struct CHICANE_RUNTIME Internals
            {
            public:
                vk::PhysicalDevice physicalDevice;
                vk::Device         logicalDevice;
                vk::SurfaceKHR     sufrace;
                vk::Instance       instance;
                vk::Queue          graphicsQueue;
                vk::CommandBuffer  mainCommandBuffer;
                SwapChain::Bundle* swapchain;
                int                imageCount;
            };

            struct CHICANE_RUNTIME Data
            {
            public:
                Vulkan::Frame::Instance frame;
                vk::CommandBuffer       commandBuffer;
                vk::Extent2D            swapChainExtent;
            };

        public:
            Renderer();
            ~Renderer();

        public:
            // Lifecycle
            void onInit() override;
            void onRender() override;
            void onEvent(const Window::Event& inEvent) override;
            void onResizing() override;
            void onRepositioning() override;

        public:
            Internals getInternals();

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