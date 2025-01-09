#pragma once

#include "Chicane/Renderer.hpp"
#include "Chicane/Renderer/Vulkan/Base.hpp"
#include "Chicane/Renderer/Vulkan/Buffer.hpp"
#include "Chicane/Renderer/Vulkan/Camera.hpp"
#include "Chicane/Renderer/Vulkan/CommandBuffer.hpp"
#include "Chicane/Renderer/Vulkan/CubeMap.hpp"
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
            Renderer(Window::Instance* inWindow);
            ~Renderer();

        public:
            void initLayers() override;

            void onEvent(const SDL_Event& inEvent) override;

            void render() override;

        public:
            Internals getInternals();

            void setViewport(
                const Vec<2, std::uint32_t>& inSize,
                const Vec<2, float>& inPosition = Vec2Zero
            );

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

            void prepareCamera(Frame::Instance& outImage);

            void renderViewport(const vk::CommandBuffer& inCommandBuffer);

            void prepareLayers(Frame::Instance& outImage);
            void renderLayers(Frame::Instance& outImage, const vk::CommandBuffer& inCommandBuffer);

            void prepareFrame(Frame::Instance& outImage);

            void refreshCameraViewport();

            void prepareEvents();

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

            // Window
            Vec<2, std::uint32_t>      m_viewportSize;
            Vec<2, float>              m_viewportPosition;
        };
    }
}