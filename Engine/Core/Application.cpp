#include "Application.h"
#include "Vertex.h"

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
            mounter.initVulkanInstance(instance, dldi);
            mounter.initVulkanDebugMessenger(debugMessenger, instance, dldi);

            // GLFW
            glfwInit();

            window = mounter.initGlfwWindow();
        }

        Application::~Application()
        {
            // Vulkan
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