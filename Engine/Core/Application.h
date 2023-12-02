#pragma once

#include "Base.h"
#include "Queue.h"
#include "Mounter/Vulkan/Instance.h"
#include "Mounter/Vulkan/Debug.h"
#include "Mounter/Vulkan/Device.h"
#include "Mounter/Vulkan/Queue.h"
#include "Mounter/GLFW/Window.h"

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

            vk::PhysicalDevice phyisicalDevice;
            vk::Device logicalDevice;
            vk::Queue graphicsQueue;

            // GLFW
            GLFWwindow* window;
        };
    }
}