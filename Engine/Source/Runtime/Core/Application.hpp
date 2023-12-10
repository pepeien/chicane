#pragma once

#include "Base.hpp"

#include "Window.hpp"

#include "Renderer/Include.hpp"

namespace Engine
{
    namespace Runtime
    {
        namespace Core
        {
            class Application
            {
            public:
                Application(std::string& inWindowTitle, Renderer::Scene& inScene);
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

                void buildGraphicsPipeline();
                void destroyGraphicsPipeline();

                void buildFramebuffers();

                void buildCommandPool();
                void destroyCommandPool();

                void buildCommandBuffers();

                void buildSyncObjects();

                void buildAssets();
                void destroyAssets();

                void buildScene(vk::CommandBuffer& inCommandBuffer);

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
                int currentFrameIndex;

                Renderer::Mesh::Manager<Renderer::Vertex::V2>* meshManager2D;

                // GLFW
                GLFWwindow* window;

                // Context
                Renderer::Scene scene;

                std::string windowTitle;
                int windowWidth;
                int windowHeight;
            };
        }
    }
}