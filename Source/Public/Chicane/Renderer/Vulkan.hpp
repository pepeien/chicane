#pragma once

#include "Chicane/Renderer.hpp"
#include "Chicane/Renderer/Vulkan/Base.hpp"
#include "Chicane/Renderer/Vulkan/Buffer.hpp"
#include "Chicane/Renderer/Vulkan/CommandBuffer.hpp"
#include "Chicane/Renderer/Vulkan/Sky.hpp"
#include "Chicane/Renderer/Vulkan/Debug.hpp"
#include "Chicane/Renderer/Vulkan/Descriptor.hpp"
#include "Chicane/Renderer/Vulkan/Device.hpp"
#include "Chicane/Renderer/Vulkan/Frame.hpp"
#include "Chicane/Renderer/Vulkan/GraphicsPipeline.hpp"
#include "Chicane/Renderer/Vulkan/Image.hpp"
#include "Chicane/Renderer/Vulkan/Instance.hpp"
#include "Chicane/Renderer/Vulkan/Queue.hpp"
#include "Chicane/Renderer/Vulkan/Shader.hpp"
#include "Chicane/Renderer/Vulkan/Surface.hpp"
#include "Chicane/Renderer/Vulkan/SwapChain.hpp"
#include "Chicane/Renderer/Vulkan/Sync.hpp"
#include "Chicane/Renderer/Vulkan/Texture.hpp"
#include "Chicane/Renderer/Vulkan/Vertex.hpp"

namespace Chicane
{
    namespace Vulkan
    {
        class Renderer : public Chicane::Renderer::Instance
        {
        public:
            struct Internals
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

            struct Data
            {
            public:
                Vulkan::Frame::Instance frame;
                vk::CommandBuffer       commandBuffer;
                vk::Extent2D            swapChainExtent;
            };

        public:
            Renderer(const Chicane::Renderer::CreateInfo& inCreateInfo, Window::Instance* inWindow);
            ~Renderer();

        public:
            // Setup
            void initLayers() override;

            // Events
            void onEvent(const SDL_Event& inEvent) override;

            // Render
            void render() override;

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
            void rebuildSwapChain();
            void destroySwapChain();

            void buildCommandPool();
            void destroyCommandPool();

            void buildMainCommandBuffer();
            void buildFramesCommandBuffers();

            void renderViewport(const vk::CommandBuffer& inCommandBuffer);

            void rebuildFrames();
            void setupFrame(Frame::Instance& outFrame);

            void setupLayers(Frame::Instance& outFrame);
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