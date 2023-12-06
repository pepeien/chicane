#pragma once

#include "Base.hpp"

#include "Window.hpp"

#include "Renderer/Classes/Buffer.hpp"
#include "Renderer/Classes/GraphicsPipeline.hpp"
#include "Renderer/Classes/Queue.hpp"
#include "Renderer/Classes/SwapChain.hpp"
#include "Renderer/Classes/Vertex.hpp"

#include "Renderer/Buffer.hpp"
#include "Renderer/Command.hpp"
#include "Renderer/Debug.hpp"
#include "Renderer/Device.hpp"
#include "Renderer/GraphicsPipeline.hpp"
#include "Renderer/Instance.hpp"
#include "Renderer/Queue.hpp"
#include "Renderer/Surface.hpp"
#include "Renderer/SwapChain.hpp"
#include "Renderer/Sync.hpp"

namespace Engine
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

            Render::SwapChain::Bundle swapChain;

            Render::GraphicsPipeline::Bundle graphicsPipeline;

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