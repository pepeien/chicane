#include "Application.hpp"

vk::ClearValue clearColor = { std::array<float, 4>{ 0.04f, 0.89f, 0.84f, 1.0f } };

namespace Engine
{
    namespace Runtime
    {
        namespace Core
        {
            Application::Application(std::string& inWindowTitle, Renderer::Scene& inScene)
            {
                windowTitle = inWindowTitle;
                scene       = inScene;

                // Stats
                numFrames   = 0;
	            frameTime   = 0.0f;
                lastTime    = 0.0;
                currentTime = 0.0;

                // GLFW
                glfwInit();
                
                buildWindow();

                // Vulkan
                buildInstance();

                buildDebugMessenger();

                buildSurface();

                buildDevices();

                buildQueues();

                buildSwapChain();

                buildGraphicsPipeline();

                buildFramebuffers();

                buildCommandPool();
                buildCommandBuffers();

                maxInFlightFramesCount = static_cast<int>(swapChain.frames.size());
                currentFrameIndex      = 0;

                buildSyncObjects();

                buildAssets();
            }

            Application::~Application()
            {
                // Vulkan
                logicalDevice.waitIdle();

                destroyGraphicsPipeline();

                destroySwapChain();

                destroyCommandPool();

                destroyAssets();

                destroyDevices();

                destroySurface();

                if (IS_DEBUGGING)
                {
                    destroyDebugMessenger();
                }

                destroyInstance();

                // GLFW
                glfwTerminate();
            }

            void Application::run()
            {
                while (glfwWindowShouldClose(window) == false)
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

                inCommandBuffer.bindPipeline(vk::PipelineBindPoint::eGraphics, graphicsPipeline.instance);

                buildScene(inCommandBuffer);

                for (glm::vec3 position : scene.positions)
                {
                    glm::mat4 model = glm::translate(glm::mat4(1.0f), position);

                    Renderer::Shader::ObjectData objectData;
                    objectData.model = model;

                    inCommandBuffer.pushConstants(
                        graphicsPipeline.layout,
                        vk::ShaderStageFlagBits::eVertex,
                        0,
                        sizeof(objectData),
                        &objectData
                    );

                    inCommandBuffer.draw(3, 1, 0, 0);
                }

                inCommandBuffer.endRenderPass();

                inCommandBuffer.end();
            }

            void Application::render()
            {
                Renderer::SwapChain::Frame currentFrame = swapChain.frames[currentFrameIndex];

                logicalDevice.waitForFences(1, &currentFrame.renderFence, VK_TRUE, UINT64_MAX);

                logicalDevice.resetFences(1, &currentFrame.renderFence);

                uint32_t imageIndex;

                try
                {
                    imageIndex = logicalDevice.acquireNextImageKHR(
                        swapChain.instance,
                        UINT64_MAX,
                        currentFrame.presentSemaphore,
                        nullptr
                    ).value;
                }
                catch(vk::OutOfDateKHRError e)
                {
                    rebuildSwapChain();

                    return;
                }
                catch(vk::IncompatibleDisplayKHRError e)
                {
                    rebuildSwapChain();

                    return;
                }

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

                vk::SwapchainKHR swapChains[] = { swapChain.instance };

                vk::PresentInfoKHR presentInfo = {};
                presentInfo.waitSemaphoreCount = 1;
                presentInfo.pWaitSemaphores    = signalSemaphores;
                presentInfo.swapchainCount     = 1;
                presentInfo.pSwapchains        = swapChains;
                presentInfo.pImageIndices      = &imageIndex;

                vk::Result present;

                try
                {
                    present = presentQueue.presentKHR(presentInfo);
                }
                catch(vk::OutOfDateKHRError e)
                {
                    present = vk::Result::eErrorOutOfDateKHR;
                }

                if (present == vk::Result::eErrorOutOfDateKHR || present == vk::Result::eSuboptimalKHR)
                {
                    rebuildSwapChain();

                    return;
                }

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

            void Application::buildWindow()
            {
                window = Window::init(
                    windowWidth,
                    windowHeight,
                    windowTitle.c_str()
                );
            }

            void Application::buildInstance()
            {
                Renderer::Instance::init(instance, dldi);
            }

            void Application::destroyInstance()
            {
                instance.destroy();
            }

            void Application::buildDebugMessenger()
            {
                if (IS_DEBUGGING == false)
                {
                    return;
                }

                Renderer::Debug::initMessenger(debugMessenger, instance, dldi);
            }

            void Application::destroyDebugMessenger()
            {
                instance.destroyDebugUtilsMessengerEXT(debugMessenger, nullptr, dldi);
            }

            void Application::buildSurface()
            {
                Renderer::Surface::init(surface, instance, window);
            }
            
            void Application::destroySurface()
            {
                instance.destroySurfaceKHR(surface);
            }

            void Application::buildQueues()
            {
                Renderer::Queue::initGraphicsQueue(
                    graphicsQueue,
                    physicalDevice,
                    logicalDevice,
                    surface
                );

                Renderer::Queue::initPresentQueue(
                    presentQueue,
                    physicalDevice,
                    logicalDevice,
                    surface
                );
            }

            void Application::buildDevices()
            {
                Renderer::Device::pickPhysicalDevice(physicalDevice, instance);
                Renderer::Device::initLogicalDevice(
                    logicalDevice,
                    physicalDevice,
                    surface
                );
            }
            
            void Application::destroyDevices()
            {
                logicalDevice.destroy();
            }

            void Application::buildSwapChain()
            {
                Renderer::SwapChain::init(
                    swapChain,
                    physicalDevice,
                    logicalDevice,
                    surface,
                    windowWidth,
                    windowHeight
                );
            }

            void Application::rebuildSwapChain()
            {
                windowWidth  = 0;
	            windowHeight = 0;

	            while (windowWidth == 0 || windowHeight == 0) {
            		glfwGetFramebufferSize(window, &windowWidth, &windowHeight);
            		glfwWaitEvents();
	            }

	            logicalDevice.waitIdle();

                destroySwapChain();
                buildSwapChain();

                destroyGraphicsPipeline();
                buildGraphicsPipeline();

                buildFramebuffers();

                buildSyncObjects();

                buildCommandBuffers();
            }

            void Application::destroySwapChain()
            {
                for (Renderer::SwapChain::Frame frame : swapChain.frames)
                {
                    logicalDevice.destroyImageView(frame.imageView);
                    logicalDevice.destroyFramebuffer(frame.framebuffer);

                    logicalDevice.destroyFence(frame.renderFence);
                    logicalDevice.destroySemaphore(frame.presentSemaphore);
                    logicalDevice.destroySemaphore(frame.renderSemaphore);
                }

                logicalDevice.destroySwapchainKHR(swapChain.instance);
            }

            void Application::buildGraphicsPipeline()
            {
                Renderer::GraphicsPipeline::CreateInfo graphicsPipelineCreateInfo = {};
                graphicsPipelineCreateInfo.logicalDevice        = logicalDevice;
                graphicsPipelineCreateInfo.vertexShaderName     = "triangle.vert.spv";
                graphicsPipelineCreateInfo.fragmentShaderName   = "triangle.frag.spv";
                graphicsPipelineCreateInfo.swapChainExtent      = swapChain.extent;
                graphicsPipelineCreateInfo.swapChainImageFormat = swapChain.format;

                Renderer::GraphicsPipeline::init(graphicsPipeline, graphicsPipelineCreateInfo);    
            }

            void Application::destroyGraphicsPipeline()
            {
                logicalDevice.destroyPipeline(graphicsPipeline.instance);
                logicalDevice.destroyPipelineLayout(graphicsPipeline.layout);
                logicalDevice.destroyRenderPass(graphicsPipeline.renderPass);
            }

            void Application::buildFramebuffers()
            {
                Renderer::Buffer::FramebufferCreateInfo framebufferCreateInfo = {
                    logicalDevice,
                    graphicsPipeline.renderPass,
                    swapChain.extent,
                    swapChain.frames
                };

                Renderer::Buffer::initFramebuffers(framebufferCreateInfo);
            }

            void Application::buildCommandPool()
            {
                Renderer::Command::initPool(
                    commandPool,
                    logicalDevice,
                    physicalDevice,
                    surface
                );
            }

            void Application::destroyCommandPool()
            {
                logicalDevice.destroyCommandPool(commandPool);
            }

            void Application::buildCommandBuffers()
            {
                Renderer::Buffer::CommandBufferCreateInfo commandBufferInfo = {
                    logicalDevice,
                    commandPool,
                    swapChain.frames
                };

                Renderer::Command::initBuffers(mainCommandBuffer, commandBufferInfo);
            }

            void Application::buildSyncObjects()
            {
                for (Renderer::SwapChain::Frame& frame : swapChain.frames)
                {
                    Renderer::Sync::initFence(frame.renderFence, logicalDevice);
                    Renderer::Sync::initSempahore(frame.presentSemaphore, logicalDevice);
                    Renderer::Sync::initSempahore(frame.renderSemaphore, logicalDevice);
                }
            }

            void Application::buildAssets()
            {
                std::vector<Renderer::Vertex::V2> triangleVertices;
                triangleVertices.resize(3);

                triangleVertices[0].position = glm::vec2(0.0f, -0.15f);
                triangleVertices[0].color    = glm::vec3(0.5f, 0.0f, 0.0f);

                triangleVertices[1].position = glm::vec2(0.15f, 0.15f);
                triangleVertices[1].color    = glm::vec3(0.0f, 0.5f, 0.0f);

                triangleVertices[2].position = glm::vec2(-0.15f, 0.15f);
                triangleVertices[2].color    = glm::vec3(0.5f, 0.0f, 0.5f);

                triangleMesh = new Renderer::Mesh(logicalDevice, physicalDevice, triangleVertices);
            }

            void Application::destroyAssets()
            {
                delete triangleMesh;
            }

            void Application::buildScene(vk::CommandBuffer& inCommandBuffer)
            {
                vk::Buffer vertexBuffers[] = { triangleMesh->vertexBuffer.instance };
                vk::DeviceSize offsets[]   = { 0 };

                inCommandBuffer.bindVertexBuffers(0, 1, vertexBuffers, offsets);
            }
        }
    }
}