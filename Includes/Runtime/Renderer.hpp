#pragma once

#include "Runtime/Runtime.hpp"
#include "Runtime/Renderer/Buffer.hpp"
#include "Runtime/Renderer/Camera.hpp"
#include "Runtime/Renderer/Debug.hpp"
#include "Runtime/Renderer/Descriptor.hpp"
#include "Runtime/Renderer/Device.hpp"
#include "Runtime/Renderer/GraphicsPipeline.hpp"
#include "Runtime/Renderer/Image.hpp"
#include "Runtime/Renderer/Instance.hpp"
#include "Runtime/Renderer/Queue.hpp"
#include "Runtime/Renderer/Shader.hpp"
#include "Runtime/Renderer/Surface.hpp"
#include "Runtime/Renderer/SwapChain.hpp"
#include "Runtime/Renderer/Sync.hpp"
#include "Runtime/Renderer/Vertex.hpp"
#include "Runtime/Renderer/CommandBuffer.hpp"
#include "Runtime/Renderer/CommandBuffer/Pool.hpp"
#include "Runtime/Renderer/CommandBuffer/Worker.hpp"
#include "Runtime/Renderer/CubeMap.hpp"
#include "Runtime/Renderer/CubeMap/Manager.hpp"
#include "Runtime/Renderer/Frame.hpp"
#include "Runtime/Renderer/Frame/Buffer.hpp"
#include "Runtime/Renderer/Model.hpp"
#include "Runtime/Renderer/Model/Manager.hpp"
#include "Runtime/Renderer/Texture.hpp"
#include "Runtime/Renderer/Texture/Manager.hpp"

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

        void destroyLayers();
        void rebuildLayers();
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
        std::unique_ptr<Camera> m_camera;
    };
}