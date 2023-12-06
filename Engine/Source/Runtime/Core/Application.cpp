#include "Application.hpp"

vk::ClearValue clearColor = { std::array<float, 4>{ 0.04f, 0.89f, 0.84f, 1.0f } };

namespace Engine
{
    namespace Runtime
    {
        namespace Core
        {
            Application::Application(std::string& inWindowTitle)
            {
                windowTitle = inWindowTitle;

                // GLFW
                glfwInit();
                window = Window::init(windowWidth, windowHeight, windowTitle.c_str());

                // Vulkan
                Renderer::Instance::init(instance, dldi);

                if (IS_DEBUGGING)
                {
                    Renderer::Debug::initMessenger(debugMessenger, instance, dldi);
                }

                Renderer::Surface::init(surface, instance, window);

                Renderer::Device::pickPhysicalDevice(physicalDevice, instance);
                Renderer::Device::initLogicalDevice(logicalDevice, physicalDevice, surface);
                Renderer::Queue::initGraphicsQueue(graphicsQueue, physicalDevice, logicalDevice, surface);
                Renderer::Queue::initPresentQueue(presentQueue, physicalDevice, logicalDevice, surface);

                Renderer::SwapChain::init(
                    swapChain,
                    physicalDevice,
                    logicalDevice,
                    surface,
                    windowWidth,
                    windowHeight
                );

                Renderer::GraphicsPipeline::CreateInfo graphicsPipelineCreateInfo = {};
                graphicsPipelineCreateInfo.logicalDevice        = logicalDevice;
                graphicsPipelineCreateInfo.vertexShaderName     = "TriangleH.vert.spv";
                graphicsPipelineCreateInfo.fragmentShaderName   = "TriangleH.frag.spv";
                graphicsPipelineCreateInfo.swapChainExtent      = swapChain.extent;
                graphicsPipelineCreateInfo.swapChainImageFormat = swapChain.format;
                Renderer::GraphicsPipeline::init(graphicsPipeline, graphicsPipelineCreateInfo);    

                Renderer::Buffer::FramebufferCreateInfo framebufferCreateInfo = {
                    logicalDevice,
                    graphicsPipeline.renderPass,
                    swapChain.extent,
                    swapChain.frames
                };

                Renderer::Buffer::initFramebuffers(framebufferCreateInfo);

                Renderer::Command::initPool(commandPool, logicalDevice, physicalDevice, surface);

                Renderer::Buffer::CommandBufferCreateInfo commandBufferInfo = {
                    logicalDevice,
                    commandPool,
                    swapChain.frames
                };

                Renderer::Command::initBuffers(mainCommandBuffer, commandBufferInfo);

                maxInFlightFramesCount = static_cast<int>(swapChain.frames.size());
                currentFrameIndex       = 0;

                for (Renderer::SwapChain::Frame& frame : swapChain.frames)
                {
                    Renderer::Sync::initFence(frame.renderFence, logicalDevice);
                    Renderer::Sync::initSempahore(frame.presentSemaphore, logicalDevice);
                    Renderer::Sync::initSempahore(frame.renderSemaphore, logicalDevice);
                }
            }

            Application::~Application()
            {
                // Vulkan
                logicalDevice.waitIdle();

                for (Renderer::SwapChain::Frame frame : swapChain.frames)
                {
                    logicalDevice.destroyFence(frame.renderFence);
                    logicalDevice.destroySemaphore(frame.presentSemaphore);
                    logicalDevice.destroySemaphore(frame.renderSemaphore);
                }

                logicalDevice.destroyCommandPool(commandPool);

                logicalDevice.destroyPipeline(graphicsPipeline.pipeline);
                logicalDevice.destroyPipelineLayout(graphicsPipeline.layout);
                logicalDevice.destroyRenderPass(graphicsPipeline.renderPass);

                for (Renderer::SwapChain::Frame frame : swapChain.frames)
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
                Renderer::SwapChain::Frame currentFrame = swapChain.frames[currentFrameIndex];

                logicalDevice.waitForFences(1, &currentFrame.renderFence, VK_TRUE, UINT64_MAX);

                logicalDevice.resetFences(1, &currentFrame.renderFence) ;

                uint32_t imageIndex {
                    logicalDevice.acquireNextImageKHR(
                        swapChain.swapchain,
                        UINT64_MAX,
                        currentFrame.presentSemaphore,
                        nullptr
                    ).value
                };

                vk::CommandBuffer commandBuffer = swapChain.frames[imageIndex].commandBuffer;

                draw(commandBuffer, imageIndex);

                vk::Semaphore waitSemaphores[]      = { currentFrame.presentSemaphore };
                vk::Semaphore signalSemaphores[]    = { currentFrame.renderSemaphore };

                vk::PipelineStageFlags waitStages[] = { vk::PipelineStageFlagBits::eColorAttachmentOutput };

                vk::SubmitInfo submitInfo = {};
                submitInfo.waitSemaphoreCount   = 1;
                submitInfo.pWaitSemaphores      = waitSemaphores;
                submitInfo.pWaitDstStageMask    = waitStages;
                submitInfo.commandBufferCount   = 1;
                submitInfo.pCommandBuffers      = &commandBuffer;
                submitInfo.signalSemaphoreCount = 1;
                submitInfo.pSignalSemaphores    = signalSemaphores;

                graphicsQueue.submit(submitInfo, currentFrame.renderFence);

                vk::SwapchainKHR swapChains[] = { swapChain.swapchain };

                vk::PresentInfoKHR presentInfo = {};
                presentInfo.waitSemaphoreCount = 1;
                presentInfo.pWaitSemaphores    = signalSemaphores;
                presentInfo.swapchainCount     = 1;
                presentInfo.pSwapchains        = swapChains;
                presentInfo.pImageIndices      = &imageIndex;

                presentQueue.presentKHR(presentInfo);

                currentFrameIndex = (currentFrameIndex + 1) % maxInFlightFramesCount;
            }

            void Application::calculateFrameRate()
            {
                currentTime = glfwGetTime();
	            double delta = currentTime - lastTime;

	            if (delta >= 1) {
	    	        int framerate{ std::max(1, int(numFrames / delta)) };

	    	        std::stringstream title;
	    	        title << windowTitle << " - " << framerate << " FPS";

	    	        glfwSetWindowTitle(window, title.str().c_str());

	    	        lastTime  = currentTime;
	    	        numFrames = -1;
	    	        frameTime = float(1000.0 / framerate);
	            }

	            ++numFrames;
            }
        }
    }
}