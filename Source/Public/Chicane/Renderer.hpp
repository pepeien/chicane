#pragma once

#include "Chicane/Base.hpp"
#include "Chicane/Renderer/Buffer.hpp"
#include "Chicane/Renderer/Camera.hpp"
#include "Chicane/Renderer/Debug.hpp"
#include "Chicane/Renderer/Descriptor.hpp"
#include "Chicane/Renderer/Device.hpp"
#include "Chicane/Renderer/GraphicsPipeline.hpp"
#include "Chicane/Renderer/Image.hpp"
#include "Chicane/Renderer/Instance.hpp"
#include "Chicane/Renderer/Queue.hpp"
#include "Chicane/Renderer/Shader.hpp"
#include "Chicane/Renderer/Surface.hpp"
#include "Chicane/Renderer/SwapChain.hpp"
#include "Chicane/Renderer/Sync.hpp"
#include "Chicane/Renderer/Vertex.hpp"
#include "Chicane/Renderer/CommandBuffer.hpp"
#include "Chicane/Renderer/CommandBuffer/Pool.hpp"
#include "Chicane/Renderer/CommandBuffer/Worker.hpp"
#include "Chicane/Renderer/CubeMap.hpp"
#include "Chicane/Renderer/CubeMap/Manager.hpp"
#include "Chicane/Renderer/Frame.hpp"
#include "Chicane/Renderer/Frame/Buffer.hpp"
#include "Chicane/Renderer/Model.hpp"
#include "Chicane/Renderer/Model/Manager.hpp"
#include "Chicane/Renderer/Texture.hpp"
#include "Chicane/Renderer/Texture/Manager.hpp"

namespace Chicane
{
    // Core
    class Layer;
    class Window;

    // Game
    class CameraComponent;

    class Renderer
    {
    public:
        struct Internals
        {
        public:
            // Vulkan
            vk::PhysicalDevice physicalDevice;
            vk::Device logicalDevice;
            vk::SurfaceKHR sufrace;
            vk::Instance instance;
            vk::Queue graphicsQueue;
            vk::CommandBuffer mainCommandBuffer;

            SwapChain::Bundle* swapchain;
            int imageCount;
        };

    public:
        Renderer(Window* inWindow);
        ~Renderer();

    public:
        Internals getInternals();

        void pushLayerStart(Layer* inLayer);
        void pushLayerBack(Layer* inLayer);
        void pushLayerBefore(const std::string& inId, Layer* inLayer);
        void pushLayerAfter(const std::string& inId, Layer* inLayer);

        void setViewport(
            const Vec<2, std::uint32_t>& inSize,
            const Vec<2, float>& inPosition = Vec<2, float>(0.0f)
        );

        // Event
        void onEvent(const SDL_Event& inEvent);

        void render();

    private:
        bool hasLayer(Layer* inLayer);
        bool hasLayer(const std::string& inId);

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

        void destroyLayers();
        void rebuildLayers();
        void deleteLayers();

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

        void buildDefaultCamera();

    private:
        // Instance
        vk::Instance m_instance;
        vk::DispatchLoaderDynamic m_dldi;
        vk::DebugUtilsMessengerEXT m_debugMessenger;

        // Surface
        vk::SurfaceKHR m_surface;

        // Devices
        vk::PhysicalDevice m_physicalDevice;
        vk::Device m_logicalDevice;

        // Queues
        vk::Queue m_graphicsQueue;
        vk::Queue m_presentQueue;

        // Swap Chain
        SwapChain::Bundle m_swapChain;

        // Command
        vk::CommandPool m_mainCommandPool;
        vk::CommandBuffer m_mainCommandBuffer;

        // Frame
        int m_imageCount;
        int m_currentImageIndex;

        // Window
        bool m_wasWindowResized;

        Window* m_window;
        Vec<2, std::uint32_t> m_viewportSize;
        Vec<2, float> m_viewportPosition;

        // Layers
        std::vector<Layer*> m_layers;

        // Essentials
        std::unique_ptr<CameraComponent> m_defaultCamera;
    };
}