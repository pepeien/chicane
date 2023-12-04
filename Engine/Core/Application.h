#pragma once

#include "Base.h"

#include "Vulkan/GraphicsPipeline.h"
#include "Vulkan/SwapChain.h"

namespace Engine
{
    namespace Core
    {
        class Application
        {
        public:
            Application();
            ~Application();

        public:
            void run();

        private:
            // Lifecycle
            void draw();

        private:
            // Vulkan
            vk::Instance instance;
            vk::DispatchLoaderDynamic dldi;
            vk::DebugUtilsMessengerEXT debugMessenger;
            vk::SurfaceKHR surface;

            vk::PhysicalDevice phyisicalDevice;
            vk::Device logicalDevice;
            vk::Queue graphicsQueue;
            vk::Queue presentQueue;

            Vulkan::SwapChain::Bundle swapChain;

            Vulkan::GraphicsPipeline::Bundle graphicsPipeline;

            // GLFW
            GLFWwindow* window;

            int windowWidth;
            int windowHeight;
        };
    }
}