#pragma once

#include "Base.h"
#include "Mounter.h"

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
            // Internals
            Mounter mounter;
            
            // Vulkan
            vk::Instance instance;

            vk::DispatchLoaderDynamic dldi;
 
            vk::DebugUtilsMessengerEXT debugMessenger;
            
            // GLFW
            GLFWwindow* window;
        };
    }
}