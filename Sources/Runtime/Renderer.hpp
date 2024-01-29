#pragma once

#include "Base.hpp"

#include "Renderer/Buffer.hpp"
#include "Renderer/Camera.hpp"
#include "Renderer/Debug.hpp"
#include "Renderer/Descriptor.hpp"
#include "Renderer/Device.hpp"
#include "Renderer/GraphicsPipeline.hpp"
#include "Renderer/Image.hpp"
#include "Renderer/Instance.hpp"
#include "Renderer/Queue.hpp"
#include "Renderer/Shader.hpp"
#include "Renderer/Surface.hpp"
#include "Renderer/SwapChain.hpp"
#include "Renderer/Sync.hpp"
#include "Renderer/Vertex.hpp"

#include "Renderer/CommandBuffer.hpp"
#include "Renderer/CommandBuffer/Pool.hpp"
#include "Renderer/CommandBuffer/Worker.hpp"

#include "Renderer/CubeMap.hpp"
#include "Renderer/CubeMap/Manager.hpp"

#include "Renderer/Frame.hpp"
#include "Renderer/Frame/Buffer.hpp"

#include "Renderer/Model.hpp"
#include "Renderer/Model/Manager.hpp"

#include "Renderer/Texture.hpp"
#include "Renderer/Texture/Manager.hpp"

namespace Chicane
{
    // Core
    class Layer;
    class Window;

    // Game
    class Actor;
    class Controller;

    class Renderer
    {
    public:
        Renderer(Window* inWindow);
        ~Renderer();

    public:
        void pushLayer(Layer* inLayer);

        void updateViewport(
            const vk::CommandBuffer& inCommandBuffer
        );

        void onEvent(const SDL_Event& inEvent);
        void render();

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

        void initLayers();
        void destroyLayers();
        void deleteLayers();

        void buildCommandPool();
        void destroyCommandPool();

        void buildMainCommandBuffer();
        void buildFramesCommandBuffers();

        void prepareCamera(Frame::Instance& outFrame);

    public:
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
        Window* m_window;

        // Layers
        std::vector<Layer*> m_layers;

        // Camera
        std::unique_ptr<Camera::Instance> m_camera;
    };
}