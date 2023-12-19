#pragma once

#include "Base.hpp"

#include "Window.hpp"

#include "Renderer.hpp"

namespace Engine
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
        void buildMeshDescriptorSetLayout();
        void buildDescriptorSetLayouts();
        void destroyDescriptorSetLayouts();

        void destroyDescriptorPools();

        void buildGraphicsPipeline();
        void destroyGraphicsPipeline();

        void buildFramebuffers();

        void buildCommandPool();
        void destroyCommandPool();

        void buildCommandBuffers();

        void buildMeshes();
        void buildTextures();
        void buildAssets();
        void destroyAssets();

        void buildFrameResources();
        void buildMeshResources();

        void prepareScene(const vk::CommandBuffer& inCommandBuffer);
        void prepareCamera(Frame::Instance& outFrame);
        void prepareModel(Frame::Instance& outFrame);
        void prepareFrame(const uint32_t& inImageIndex);

        void drawObjects(const vk::CommandBuffer& inCommandBuffer);

    private:
        // Stats
        int numFrames;
        float frameTime;
        double lastTime;
        double currentTime;

        // Vulkan
        vk::Instance instance;
        vk::DispatchLoaderDynamic dldi;
        vk::DebugUtilsMessengerEXT debugMessenger;
        vk::SurfaceKHR surface;

        vk::PhysicalDevice physicalDevice;
        vk::Device logicalDevice;
        vk::Queue graphicsQueue;
        vk::Queue presentQueue;

        SwapChain::Bundle swapChain;

        GraphicsPipeline::Bundle graphicsPipeline;

        vk::CommandPool commandPool;
        vk::CommandBuffer mainCommandBuffer;

        int maxInFlightFramesCount;
        int currentImageIndex;

        vk::DescriptorSetLayout frameDescriptorSetLayout;
        vk::DescriptorPool frameDescriptorPool;

        vk::DescriptorSetLayout meshDescriptorSetLayout;
        vk::DescriptorPool meshDescriptorPool;

        Mesh::Manager::Instance* meshManager;
        Texture::Manager::Instance* textureManager;

        // GLFW
        GLFWwindow* window;

        // Context
        Scene::Instance scene;

        std::string windowTitle;
        int windowWidth;
        int windowHeight;
    };
}