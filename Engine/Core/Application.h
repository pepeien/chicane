#pragma once

#include <algorithm>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <limits>
#include <set>
#include <stdexcept>
#include <vector>

#include <vulkan/vulkan.h>
#include <GLFW/glfw3.h>

#include "QueueFamily.h"
#include "SwapChain.h"

namespace Engine
{
    namespace Core
    {
        class Application
        {
        public:
            void run();

        private:
            bool checkValidationLayerSupport();
            bool doesSupportExtensions(VkPhysicalDevice device);
            bool isSuitable(VkPhysicalDevice device);

            // Setup
            void selectPhysicalDevice();
            void initLogicalDevice();
            void initInstance();

            // Presentation
            QueueFamilyIndices getQueueFamily(VkPhysicalDevice device);
            void initSurface();

            VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities);
            VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);
            VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
            SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device);
            void initSwapChain();

            void initImageViews();

            // Graphic Pipeline
            VkShaderModule initShaderModule(const std::vector<char>& code);
            void initRenderPass();
            void initGraphicsPipeline();

            // Drawing
            void initFrameBuffers();
            void initCommandPool();
            void initCommandBuffer();
            void writeCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex);
            void initSyncObjects();

            // Lifecycle
            void initVulkan();
            void cleanupVulkan();

            void initGLFW();
            void cleanupGLFW();

            void drawFrame();
            void loop();
            void cleanup();

        private:
            // Vulkan
            VkPhysicalDevice physicalDevice;
            VkDevice selectedDevice;

            VkQueue graphicsQueue;
            VkQueue presentQueue;

            std::vector<VkImageView> swapChainImageViews;
            std::vector<VkImage> swapChainImages;
            VkFormat swapChainImageFormat;
            VkExtent2D swapChainExtent;
            VkSwapchainKHR swapChain;

            VkRenderPass renderPass;
            VkPipelineLayout pipelineLayout;
            VkPipeline graphicsPipeline;

            std::vector<VkFramebuffer> swapChainFramebuffers;
            VkCommandPool commandPool;
            VkCommandBuffer commandBuffer;

            VkSemaphore imageAvailableSemaphore;
            VkSemaphore renderFinishedSemaphore;
            VkFence inFlightFence;

            VkSurfaceKHR surface;

            VkInstance instance;

            // GLFW
            GLFWwindow* window;
        };
    }
}