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

        Buffer::Instance meshVertexBuffer;
        Buffer::Instance meshIndexBuffer;
        std::unique_ptr<Mesh::Manager::Instance> meshManager;

        std::unique_ptr<Texture::Manager::Instance> textureManager;

        vk::DescriptorSetLayout materialDescriptorSetLayout;
        vk::DescriptorPool materialDescriptorPool;

        // GLFW
        GLFWwindow* window;

        // Context
        Scene::Instance scene;

        std::string windowTitle;
        int windowWidth;
        int windowHeight;
    };
}