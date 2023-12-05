#include "Application.hpp"

vk::ClearValue clearColor = { std::array<float, 4>{ 0.04f, 0.89f, 0.84f, 1.0f } };

namespace Engine
{
    namespace Core
    {
        Application::Application()
        {
            // GLFW
            glfwInit();
            window = Window::init(windowWidth, windowHeight);

            // Vulkan
            Render::Instance::init(instance, dldi);
            
            if (IS_DEBUGGING)
            {
                Render::Debug::initMessenger(debugMessenger, instance, dldi);
            }

            Render::Surface::init(surface, instance, window);

            Render::Device::pickPhysicalDevice(physicalDevice, instance);
            Render::Device::initLogicalDevice(logicalDevice, physicalDevice, surface);
            Render::Queue::initGraphicsQueue(graphicsQueue, physicalDevice, logicalDevice, surface);
            Render::Queue::initPresentQueue(presentQueue, physicalDevice, logicalDevice, surface);

            Render::SwapChain::init(
                swapChain,
                physicalDevice,
                logicalDevice,
                surface,
                windowWidth,
                windowHeight
            );

            Render::GraphicsPipeline::CreateInfo graphicsPipelineCreateInfo = {};
            graphicsPipelineCreateInfo.logicalDevice        = logicalDevice;
            graphicsPipelineCreateInfo.vertexShaderName     = "TriangleH.vert.spv";
            graphicsPipelineCreateInfo.fragmentShaderName   = "TriangleH.frag.spv";
            graphicsPipelineCreateInfo.swapChainExtent      = swapChain.extent;
            graphicsPipelineCreateInfo.swapChainImageFormat = swapChain.format;
            Render::GraphicsPipeline::init(graphicsPipeline, graphicsPipelineCreateInfo);    

            Render::Buffer::FramebufferCreateInfo framebufferCreateInfo = {
                logicalDevice,
                graphicsPipeline.renderPass,
                swapChain.extent,
                swapChain.frames
            };
    
            Render::Buffer::initFramebuffers(framebufferCreateInfo);

            Render::Command::initPool(commandPool, logicalDevice, physicalDevice, surface);

            Render::Buffer::CommandBufferCreateInfo commandBufferInfo = {
                logicalDevice,
                commandPool,
                swapChain.frames
            };

            Render::Command::initBuffers(mainCommandBuffer, commandBufferInfo);

            Render::Sync::initFence(inFlightFence, logicalDevice);
            Render::Sync::initSempahore(imageAvailableSemaphore, logicalDevice);
            Render::Sync::initSempahore(renderFinishedSemaphore, logicalDevice);
        }

        Application::~Application()
        {
            // Vulkan
            logicalDevice.waitIdle();

            logicalDevice.destroyFence(inFlightFence);
            logicalDevice.destroySemaphore(imageAvailableSemaphore);
            logicalDevice.destroySemaphore(renderFinishedSemaphore);

            logicalDevice.destroyCommandPool(commandPool);

            logicalDevice.destroyPipeline(graphicsPipeline.pipeline);
            logicalDevice.destroyPipelineLayout(graphicsPipeline.layout);
            logicalDevice.destroyRenderPass(graphicsPipeline.renderPass);

            for (Render::SwapChain::Frame frame : swapChain.frames)
            {
                logicalDevice.destroyImageView(frame.imageView);
                logicalDevice.destroyFramebuffer(frame.framebuffer);
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
                render();
                calculateFrameRate();
            }
        }
        
        void Application::draw(vk::CommandBuffer& inCommandBuffer, uint32_t inImageIndex)
        {
            vk::CommandBufferBeginInfo beginInfo = {};

            inCommandBuffer.begin(beginInfo);

            vk::RenderPassBeginInfo renderPassBeginInfo = {};
            renderPassBeginInfo.renderPass          = graphicsPipeline.renderPass;
            renderPassBeginInfo.framebuffer         = swapChain.frames[inImageIndex].framebuffer;
            renderPassBeginInfo.renderArea.offset.x = 0;
            renderPassBeginInfo.renderArea.offset.y = 0;
            renderPassBeginInfo.renderArea.extent   = swapChain.extent;
            renderPassBeginInfo.clearValueCount     = 1;
            renderPassBeginInfo.pClearValues        = &clearColor;

            inCommandBuffer.beginRenderPass(&renderPassBeginInfo, vk::SubpassContents::eInline);

            inCommandBuffer.bindPipeline(vk::PipelineBindPoint::eGraphics, graphicsPipeline.pipeline);

            inCommandBuffer.draw(3, 1, 0, 0);

            inCommandBuffer.endRenderPass();

            inCommandBuffer.end();
        }

        void Application::render()
        {
            logicalDevice.waitForFences(1, &inFlightFence, VK_TRUE, UINT64_MAX);

            logicalDevice.resetFences(1, &inFlightFence) ;

            uint32_t imageIndex {
                logicalDevice.acquireNextImageKHR(swapChain.swapchain, UINT64_MAX, imageAvailableSemaphore, nullptr).value
            };

            vk::CommandBuffer commandBuffer = swapChain.frames[imageIndex].commandBuffer;

            draw(commandBuffer, imageIndex);

            vk::Semaphore waitSemaphores[]      = { imageAvailableSemaphore };
            vk::Semaphore signalSemaphores[]    = { renderFinishedSemaphore };

            vk::PipelineStageFlags waitStages[] = { vk::PipelineStageFlagBits::eColorAttachmentOutput };

            vk::SubmitInfo submitInfo = {};
            submitInfo.waitSemaphoreCount   = 1;
            submitInfo.pWaitSemaphores      = waitSemaphores;
            submitInfo.pWaitDstStageMask    = waitStages;
            submitInfo.commandBufferCount   = 1;
            submitInfo.pCommandBuffers      = &commandBuffer;
            submitInfo.signalSemaphoreCount = 1;
            submitInfo.pSignalSemaphores    = signalSemaphores;

            graphicsQueue.submit(submitInfo, inFlightFence);

            vk::SwapchainKHR swapChains[] = { swapChain.swapchain };

            vk::PresentInfoKHR presentInfo = {};
            presentInfo.waitSemaphoreCount = 1;
            presentInfo.pWaitSemaphores    = signalSemaphores;
            presentInfo.swapchainCount     = 1;
            presentInfo.pSwapchains        = swapChains;
            presentInfo.pImageIndices      = &imageIndex;

            presentQueue.presentKHR(presentInfo);
        }

        void Application::calculateFrameRate()
        {
            currentTime = glfwGetTime();
	        double delta = currentTime - lastTime;

	        if (delta >= 1) {
		        int framerate{ std::max(1, int(numFrames / delta)) };

		        std::stringstream title;
		        title << APPLICATION_NAME << " - " << framerate << " FPS";

		        glfwSetWindowTitle(window, title.str().c_str());

		        lastTime  = currentTime;
		        numFrames = -1;
		        frameTime = float(1000.0 / framerate);
	        }

	        ++numFrames;
        }
    }
}