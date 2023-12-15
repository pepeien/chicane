#pragma once

#include "Base.hpp"

#include "Window.hpp"

#include "Renderer.hpp"

namespace Engine
{
    namespace Runtime
    {
        namespace Core
        {
            class Application
            {
            public:
                Application(std::string& inWindowTitle, Renderer::Scene::Instance& inScene);
                ~Application();

            public:
                void run();

            private:
                void draw(vk::CommandBuffer& inCommandBuffer, uint32_t inImageIndex);
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

                void buildDescriptorSetLayout();
                void destroyDescriptorSetLayout();

                void buildGraphicsPipeline();
                void destroyGraphicsPipeline();

                void buildFramebuffers();

                void buildCommandPool();
                void destroyCommandPool();

                void buildCommandBuffers();

                void buildFrameResources();

                void buildAssets();
                void destroyAssets();

                void prepareScene(vk::CommandBuffer& inCommandBuffer);
                void prepareCamera(Renderer::SwapChain::Frame& outFrame);
                void prepareModel(Renderer::SwapChain::Frame& outFrame);
                void prepareFrame(uint32_t inImageIndex);

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

                Renderer::SwapChain::Bundle swapChain;

                Renderer::GraphicsPipeline::Bundle graphicsPipeline;

                vk::CommandPool commandPool;
                vk::CommandBuffer mainCommandBuffer;

                int maxInFlightFramesCount;
                int currentImageIndex;

                vk::DescriptorSetLayout descriptorSetLayout;
                vk::DescriptorPool descriptorPool;

                Renderer::Mesh::Manager* meshManager;

                // GLFW
                GLFWwindow* window;

                // Context
                Renderer::Scene::Instance scene;

                std::string windowTitle;
                int windowWidth;
                int windowHeight;
            };
        }
    }
}