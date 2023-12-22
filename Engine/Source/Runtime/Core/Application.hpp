#pragma once

#include "Base.hpp"

#include "Window.hpp"

#include "Renderer.hpp"

namespace Chicane
{
    class Application
    {
    public:
        Application(const std::string& inWindowTitle, const Scene::Instance& inScene);
        ~Application();

    public:
        void run();

    private:
        void draw(const vk::CommandBuffer& inCommandBuffer, const uint32_t& inImageIndex);
        void render();
        void calculateFrameRate();

        // GLFW
        void buildWindow();

        // Vulkan
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

        void buildFrameDescriptorSetLayout();

        void buildMaterialDescriptorSetLayout();

        void buildGraphicsPipeline();
        void destroyGraphicsPipeline();

        void buildFramebuffers();

        void buildCommandPool();
        void destroyCommandPool();

        void buildMainCommandBuffer();
        void buildFramesCommandBuffers();

        void loadMeshes();
        void buildMeshes();

        void loadTextures();
        void buildTextures();
        void destroyTextures();

        void loadAssets();
        void buildAssets();
        void destroyAssets();

        void buildFrameResources();

        void buildMaterialResources();

        void prepareScene(const vk::CommandBuffer& inCommandBuffer);
        void prepareCamera(Frame::Instance& outFrame);
        void prepareSceneObjects(Frame::Instance& outFrame);
        void prepareFrame(const uint32_t& inImageIndex);

        void drawObjects(const vk::CommandBuffer& inCommandBuffer);

    private:
        // Stats
        Frame::Stats m_frameStats;

        // Vulkan
        vk::Instance m_instance;
        vk::DispatchLoaderDynamic m_dldi;
        vk::DebugUtilsMessengerEXT m_debugMessenger;
        vk::SurfaceKHR m_surface;

        vk::PhysicalDevice m_physicalDevice;
        vk::Device m_logicalDevice;
        vk::Queue m_graphicsQueue;
        vk::Queue m_presentQueue;

        SwapChain::Bundle m_swapChain;

        GraphicsPipeline::Bundle m_graphicsPipeline;

        vk::CommandPool m_mainCommandPool;
        vk::CommandBuffer m_mainCommandBuffer;

        int m_maxInFlightFramesCount;
        int m_currentImageIndex;

        Descriptor::Bundle m_frameDescriptors;
        Descriptor::Bundle m_materialDescriptors;

        Buffer::Instance m_meshVertexBuffer;
        Buffer::Instance m_meshIndexBuffer;
    
        std::unique_ptr<Mesh::Manager::Instance> m_meshManager;
        std::unique_ptr<Texture::Manager::Instance> m_textureManager;

        // Context
        Scene::Instance m_scene;

        // Window
        Window::Instance m_window;
    };
}