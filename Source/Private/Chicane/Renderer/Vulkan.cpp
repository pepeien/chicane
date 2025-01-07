#include "Chicane/Renderer/Vulkan.hpp"

#include "Chicane/Application.hpp"
#include "Chicane/Core.hpp"
#include "Chicane/Game.hpp"
#include "Chicane/Renderer/Vulkan/Layer/LevelLayer.hpp"
#include "Chicane/Renderer/Vulkan/Layer/UILayer.hpp"
#include "Chicane/Renderer/Vulkan/Layer/SkyboxLayer.hpp"

namespace Chicane
{
    namespace Vulkan
    {
        Renderer::Renderer(Window* inWindow)
            : Chicane::Renderer(inWindow),
            m_swapChain({}),
            m_imageCount(0),
            m_currentImageIndex(0),
            m_viewportSize(Vec<2, std::uint32_t>(0)),
            m_viewportPosition(Vec<2, std::uint32_t>(0))
        {
            buildInstance();
            buildDebugMessenger();
            buildSurface();
            buildDevices();
            buildQueues();
            buildSwapChain();
            buildCommandPool();
            buildMainCommandBuffer();
            buildFramesCommandBuffers();

            prepareEvents();
        }

        Renderer::~Renderer()
        {
            m_logicalDevice.waitIdle();

            // Vulkan
            destroyCommandPool();
            destroySwapChain();
            deleteLayers();

            Loader::reset();

            destroyDevices();
            destroySurface();

            if (IS_DEBUGGING)
            {
                destroyDebugMessenger();
            }

            destroyInstance();
        }

        Renderer::Internals Renderer::getInternals()
        {
            Internals internals {};
            internals.physicalDevice    = m_physicalDevice;
            internals.logicalDevice     = m_logicalDevice;
            internals.sufrace           = m_surface;
            internals.instance          = m_instance;
            internals.graphicsQueue     = m_graphicsQueue;
            internals.swapchain         = &m_swapChain;
            internals.mainCommandBuffer = m_mainCommandBuffer;
            internals.imageCount        = m_imageCount;

            return internals;
        }

        void Renderer::setViewport(
            const Vec<2, std::uint32_t>& inSize,
            const Vec<2, float>& inPosition
        )
        {
            m_viewportSize     = inSize;
            m_viewportPosition = inPosition;

            refreshCameraViewport();
        }

        void Renderer::initLayers()
        {
            //pushLayer(new SkyboxLayer());
            pushLayer(new LevelLayer());
            pushLayer(new UILayer());
        }

        void Renderer::onEvent(const SDL_Event& inEvent)
        {
            if (inEvent.type == SDL_EVENT_WINDOW_RESIZED)
            {
                rebuildSwapChain();
            }

            emmitEventToLayers(inEvent);
        }

        void Renderer::render()
        {
            Frame::Instance& currentImage           = m_swapChain.frames.at(m_currentImageIndex);
            vk::CommandBuffer& currentCommandBuffer = currentImage.commandBuffer;

            currentImage.wait(m_logicalDevice);

            vk::ResultValue<std::uint32_t> acquireResult = currentImage.getNextIndex(
                m_swapChain.instance,
                m_logicalDevice
            );

            if (acquireResult.result == vk::Result::eErrorOutOfDateKHR)
            {
                rebuildSwapChain();

                return;
            } else if (acquireResult.result != vk::Result::eSuccess && acquireResult.result != vk::Result::eSuboptimalKHR)
            {
                throw std::runtime_error("Error while acquiring the next image");
            }

            currentImage.reset(m_logicalDevice);

            std::uint32_t nextImageIndex = acquireResult.value;
            Frame::Instance& nextImage   = m_swapChain.frames.at(nextImageIndex);

            prepareCamera(currentImage);
            prepareLayers(currentImage);

            vk::CommandBufferBeginInfo commandBufferBegin {};
            commandBufferBegin.flags = vk::CommandBufferUsageFlagBits::eOneTimeSubmit;

            currentCommandBuffer.reset();
            currentCommandBuffer.begin(commandBufferBegin);
                prepareFrame(nextImage);
                renderViewport(currentCommandBuffer);
                renderLayers(nextImage, currentCommandBuffer);
            currentCommandBuffer.end();

            vk::PipelineStageFlags waitStages[] = { vk::PipelineStageFlagBits::eColorAttachmentOutput };
        
            vk::SubmitInfo submitInfo {};
            submitInfo.waitSemaphoreCount   = 1;
            submitInfo.pWaitSemaphores      = &currentImage.presentSemaphore;
            submitInfo.commandBufferCount   = 1;
            submitInfo.pCommandBuffers      = &currentCommandBuffer;
            submitInfo.signalSemaphoreCount = 1;
            submitInfo.pSignalSemaphores    = &currentImage.renderSemaphore;
            submitInfo.pWaitDstStageMask    = waitStages;

            vk::Result submitResult = m_graphicsQueue.submit(
                1,
                &submitInfo,
                currentImage.renderFence
            );

            if (submitResult != vk::Result::eSuccess)
            {
                throw std::runtime_error("Error while submiting the next image");
            }

            vk::PresentInfoKHR presentInfo {};
            presentInfo.waitSemaphoreCount = 1;
            presentInfo.pWaitSemaphores    = &currentImage.renderSemaphore;
            presentInfo.swapchainCount     = 1;
            presentInfo.pSwapchains        = &m_swapChain.instance;
            presentInfo.pImageIndices      = &nextImageIndex;

            vk::Result presentResult = m_presentQueue.presentKHR(presentInfo);

            if (presentResult == vk::Result::eErrorOutOfDateKHR || presentResult == vk::Result::eSuboptimalKHR)
            {
                rebuildSwapChain();
            } else if (presentResult != vk::Result::eSuccess)
            {
                throw std::runtime_error("Error while presenting the image");
            }

            m_currentImageIndex = (m_currentImageIndex + 1) % m_imageCount;
        }

        void Renderer::buildInstance()
        {
            Instance::init(
                m_instance,
                m_dldi,
                m_window->instance
            );
        }

        void Renderer::destroyInstance()
        {
            m_instance.destroy();
        }

        void Renderer::buildDebugMessenger()
        {
            if (!IS_DEBUGGING)
            {
                return;
            }

            Debug::initMessenger(
                m_debugMessenger,
                m_instance,
                m_dldi
            );
        }

        void Renderer::destroyDebugMessenger()
        {
            m_instance.destroyDebugUtilsMessengerEXT(
                m_debugMessenger,
                nullptr,
                m_dldi
            );
        }

        void Renderer::buildSurface()
        {
            Surface::init(
                m_surface,
                m_instance,
                m_window->instance
            );
        }

        void Renderer::destroySurface()
        {
            m_instance.destroySurfaceKHR(m_surface);
        }

        void Renderer::buildQueues()
        {
            Queue::initGraphicsQueue(
                m_graphicsQueue,
                m_physicalDevice,
                m_logicalDevice,
                m_surface
            );

            Queue::initPresentQueue(
                m_presentQueue,
                m_physicalDevice,
                m_logicalDevice,
                m_surface
            );
        }

        void Renderer::buildDevices()
        {
            Device::pickPhysicalDevice(
                m_physicalDevice,
                m_instance
            );
            Device::initLogicalDevice(
                m_logicalDevice,
                m_physicalDevice,
                m_surface
            );
        }
                    
        void Renderer::destroyDevices()
        {
            m_logicalDevice.destroy();
        }

        void Renderer::buildSwapChain()
        {
            SwapChain::init(
                m_swapChain,
                m_physicalDevice,
                m_logicalDevice,
                m_surface,
                m_window->getDrawableSize()
            );

            m_imageCount = static_cast<int>(m_swapChain.frames.size());

            for (Frame::Instance& frame : m_swapChain.frames)
            {
                frame.setupSync();
            }

            setViewport(
                Vec<2, std::uint32_t>(
                    m_swapChain.extent.width,
                    m_swapChain.extent.height
                )
            );
        }

        void Renderer::rebuildSwapChain()
        {
            if (m_window->isMinimized())
            {
                return;
            }

            m_logicalDevice.waitIdle();

            destroySwapChain();
            buildSwapChain();

            rebuildLayers();

            buildFramesCommandBuffers();
        }

        void Renderer::destroySwapChain()
        {
            for (Frame::Instance& frame : m_swapChain.frames)
            {
                frame.destroy();
            }

            m_swapChain.frames.clear();

            m_logicalDevice.destroySwapchainKHR(m_swapChain.instance);

            destroyLayers();
        }

        void Renderer::buildCommandPool()
        {
            CommandBuffer::Pool::init(
                m_mainCommandPool,
                m_logicalDevice,
                m_physicalDevice,
                m_surface
            );
        }

        void Renderer::destroyCommandPool()
        {
            m_logicalDevice.destroyCommandPool(m_mainCommandPool);
        }

        void Renderer::buildMainCommandBuffer()
        {
            CommandBuffer::CreateInfo createInfo = {
                m_logicalDevice,
                m_mainCommandPool
            };

            CommandBuffer::init(
                m_mainCommandBuffer,
                createInfo
            );
        }

        void Renderer::buildFramesCommandBuffers()
        {
            CommandBuffer::CreateInfo createInfo = {
                m_logicalDevice,
                m_mainCommandPool
            };

            Frame::Buffer::initCommand(
                m_swapChain.frames,
                createInfo
            );
        }

        void Renderer::prepareCamera(Frame::Instance& outImage)
        {
            if (!Application::hasCamera())
            {
                return;
            }

            outImage.cameraUBO.instance = Application::getCamera()->getUBO();
            memcpy(
                outImage.cameraUBO.writeLocation,
                &outImage.cameraUBO.instance,
                outImage.cameraUBO.allocationSize
            );
        }

        void Renderer::renderViewport(const vk::CommandBuffer& inCommandBuffer)
        {
            vk::Viewport viewport = GraphicsPipeline::createViewport(
                m_viewportSize,
                m_viewportPosition
            );
            inCommandBuffer.setViewport(0, 1, &viewport);

            vk::Rect2D scissor = GraphicsPipeline::createScissor(m_viewportSize);
            inCommandBuffer.setScissor(0, 1, &scissor);
        }

        void Renderer::refreshCameraViewport()
        {
            if (!Application::hasCamera())
            {
                return;
            }

            Application::getCamera()->setViewport(
                m_viewportSize.x,
                m_viewportSize.y
            );
        }

        void Renderer::prepareLayers(Frame::Instance& outImage)
        {
            for (Layer* layer : m_layers)
            {
                layer->setup(&outImage);
            }
        }

        void Renderer::renderLayers(Frame::Instance& outImage, const vk::CommandBuffer& inCommandBuffer)
        {
            Data data = {};
            data.commandBuffer   = inCommandBuffer;
            data.frame           = outImage;
            data.swapChainExtent = m_swapChain.extent;

            for (Layer* layer : m_layers)
            {
                layer->render(&data);
            }
        }

        void Renderer::prepareFrame(Frame::Instance& outImage)
        {
            outImage.updateDescriptorSets();
        }

        void Renderer::prepareEvents()
        {
            Application::watchCamera(
                [this](CameraComponent* inCamera) {
                    if (!inCamera)
                    {
                        return;
                    }

                    refreshCameraViewport();
                }
            );
        }
    }
}