#include "Application.h"

#include "GLFW/Mounter/Window.h"

#include "Vulkan/Queue.h"
#include "Vulkan/Vertex.h"

#include "Vulkan/Mounter/Instance.h"
#include "Vulkan/Mounter/Debug.h"
#include "Vulkan/Mounter/Device.h"
#include "Vulkan/Mounter/GraphicsPipeline.h"
#include "Vulkan/Mounter/Queue.h"
#include "Vulkan/Mounter/Surface.h"
#include "Vulkan/Mounter/SwapChain.h"

const std::vector<Engine::Core::Vulkan::Vertex> vertices = {
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
            // GLFW
            glfwInit();
            window = GLFW::Mounter::initWindow(windowWidth, windowHeight);

            // Vulkan
            Vulkan::Mounter::initInstance(instance, dldi);
            
            if (IS_DEBUGGING)
            {
                Vulkan::Mounter::initDebugMessenger(debugMessenger, instance, dldi);
            }

            Vulkan::Mounter::initSurface(surface, instance, window);

            Vulkan::Mounter::pickPhysicalDevice(phyisicalDevice, instance);
            Vulkan::Mounter::initLogicalDevice(logicalDevice, phyisicalDevice, surface);
            Vulkan::Mounter::initGraphicsQueue(graphicsQueue, phyisicalDevice, logicalDevice, surface);
            Vulkan::Mounter::initPresentQueue(presentQueue, phyisicalDevice, logicalDevice, surface);

            Vulkan::Mounter::initSwapChain(
                swapChain,
                phyisicalDevice,
                logicalDevice,
                surface,
                windowWidth,
                windowHeight
            );

            Vulkan::GraphicsPipeline::CreateInfo graphicsPipelineCreateInfo = {};
            graphicsPipelineCreateInfo.logicalDevice        = logicalDevice;
            graphicsPipelineCreateInfo.vertexShaderName     = "TriangleH.vert.spv";
            graphicsPipelineCreateInfo.fragmentShaderName   = "TriangleH.frag.spv";
            graphicsPipelineCreateInfo.swapChainExtent      = swapChain.extent;
            graphicsPipelineCreateInfo.swapChainImageFormat = swapChain.format;

            Vulkan::Mounter::initGraphicsPipeline(graphicsPipeline, graphicsPipelineCreateInfo);           
        }

        Application::~Application()
        {
            // Vulkan
            logicalDevice.destroyPipeline(graphicsPipeline.pipeline);
            logicalDevice.destroyPipelineLayout(graphicsPipeline.layout);
            logicalDevice.destroyRenderPass(graphicsPipeline.renderPass);

            for (Vulkan::SwapChain::Frame frame : swapChain.frames)
            {
                logicalDevice.destroyImageView(frame.imageView);
            }

            logicalDevice.destroySwapchainKHR(swapChain.swapchain);
            logicalDevice.destroy();

            if (IS_DEBUGGING)
            {
                instance.destroyDebugUtilsMessengerEXT(debugMessenger, nullptr, dldi);
            }

            instance.destroySurfaceKHR(surface);
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