#pragma once

#include "Base.hpp"

#include "Window.hpp"

#include "Renderer/Classes/Include.hpp"

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
                Application(std::string& inWindowTitle);
                ~Application();

            public:
                void run();

            private:
                void draw(vk::CommandBuffer& inCommandBuffer, uint32_t inImageIndex);
                void render();
                void calculateFrameRate();

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

                // GLFW
                GLFWwindow* window;

                std::string windowTitle;
                int windowWidth;
                int windowHeight;
            };
        }
    }
}