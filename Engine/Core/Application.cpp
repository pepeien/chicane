#include "Application.h"
#include "Vertex.h"
#include "Queue.h"

const std::vector<Engine::Core::Vertex> vertices = {
    {{0.0f, -0.5f}, {1.0f, 0.0f, 0.0f}},
    {{0.5f, 0.5f},  {0.0f, 1.0f, 0.0f}},
    {{-0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}}
};

namespace Engine
{
    namespace Core
    {
        Application::Application()
        {
            // Vulkan
            Mounter::Vulkan::initInstance(instance, dldi);
            
            if (IS_DEBUGGING)
            {
                Mounter::Vulkan::initDebugMessenger(debugMessenger, instance, dldi);
            }

            Mounter::Vulkan::pickPhysicalDevice(phyisicalDevice, instance);
            Mounter::Vulkan::initLogicalDevice(logicalDevice, phyisicalDevice);
            Mounter::Vulkan::initGraphicsQueue(graphicsQueue, phyisicalDevice, logicalDevice);

            // GLFW
            glfwInit();

            window = Mounter::GLFW::initWindow();
        }

        Application::~Application()
        {
            // Vulkan
            logicalDevice.destroy();

            if (IS_DEBUGGING)
            {
                instance.destroyDebugUtilsMessengerEXT(debugMessenger, nullptr, dldi);
            }

            instance.destroy();

            // GLFW
            glfwTerminate();
        }

        void Application::run()
        {
            while (!glfwWindowShouldClose(window))
            {
                glfwPollEvents();
                draw();
            }
        }
        
        void Application::draw()
        {
            
        }
    }
}