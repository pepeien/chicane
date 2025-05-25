#pragma once

#include "Chicane/Runtime/Renderer.hpp"
#include "Chicane/Runtime/Renderer/Vulkan/Essential.hpp"
#include "Chicane/Runtime/Renderer/Vulkan/Buffer.hpp"
#include "Chicane/Runtime/Renderer/Vulkan/CommandBuffer.hpp"
#include "Chicane/Runtime/Renderer/Vulkan/Sky.hpp"
#include "Chicane/Runtime/Renderer/Vulkan/Debug.hpp"
#include "Chicane/Runtime/Renderer/Vulkan/Descriptor.hpp"
#include "Chicane/Runtime/Renderer/Vulkan/Device.hpp"
#include "Chicane/Runtime/Renderer/Vulkan/Frame.hpp"
#include "Chicane/Runtime/Renderer/Vulkan/GraphicsPipeline.hpp"
#include "Chicane/Runtime/Renderer/Vulkan/Image.hpp"
#include "Chicane/Runtime/Renderer/Vulkan/Instance.hpp"
#include "Chicane/Runtime/Renderer/Vulkan/Queue.hpp"
#include "Chicane/Runtime/Renderer/Vulkan/Shader.hpp"
#include "Chicane/Runtime/Renderer/Vulkan/Surface.hpp"
#include "Chicane/Runtime/Renderer/Vulkan/SwapChain.hpp"
#include "Chicane/Runtime/Renderer/Vulkan/Sync.hpp"
#include "Chicane/Runtime/Renderer/Vulkan/Texture.hpp"
#include "Chicane/Runtime/Renderer/Vulkan/Vertex.hpp"

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
            void onEvent(const SDL_Event& inEvent) override;
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
            vk::Instance               m_instance;
            vk::DispatchLoaderDynamic  m_dldi;
            vk::DebugUtilsMessengerEXT m_debugMessenger;

            // Surface
            vk::SurfaceKHR             m_surface;

            // Devices
            vk::PhysicalDevice         m_physicalDevice;
            vk::Device                 m_logicalDevice;

            // Queues
            vk::Queue                  m_graphicsQueue;
            vk::Queue                  m_presentQueue;

            // Swap Chain
            SwapChain::Bundle          m_swapChain;

            // Command
            vk::CommandPool            m_mainCommandPool;
            vk::CommandBuffer          m_mainCommandBuffer;

            // Frame
            int                        m_imageCount;
            int                        m_currentImageIndex;

            // Viewport
            vk::Viewport               m_vkViewport;
            vk::Rect2D                 m_vkScissor;
        };
    }
}