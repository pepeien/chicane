#include "Renderer.hpp"

#include "Core.hpp"
#include "Game.hpp"

namespace Chicane
{
    Renderer::Renderer(Window* inWindow)
        : m_swapChain({}),
        m_imageCount(0),
        m_currentImageIndex(0),
        m_window(inWindow),
        m_camera(std::make_unique<Camera>())
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
    }

    Renderer::~Renderer()
    {
        // Vulkan
        m_logicalDevice.waitIdle();

        destroyCommandPool();
        destroySwapChain();
        deleteLayers();

        m_camera.reset();

        destroyDevices();
        destroySurface();

        if (IS_DEBUGGING)
        {
            destroyDebugMessenger();
        }

        destroyInstance();
    }

    void Renderer::pushLayer(Layer* inLayer)
    {
        m_layers.push_back(inLayer);

        inLayer->build();
    }

    void Renderer::updateViewport(const vk::CommandBuffer& inCommandBuffer)
    {
        vk::Viewport viewport = GraphicsPipeline::createViewport(m_swapChain.extent);
        inCommandBuffer.setViewport(0, 1, &viewport);

        vk::Rect2D scissor = GraphicsPipeline::createScissor(m_swapChain.extent);
        inCommandBuffer.setScissor(0, 1, &scissor);
    }

    void Renderer::onEvent(const SDL_Event& inEvent)
    {
        for (Layer* layer : m_layers)
        {
            layer->onEvent(inEvent);
        }
    }

    void Renderer::render()
    {
        m_logicalDevice.waitIdle();

        Frame::Instance& currentImage = m_swapChain.images[m_currentImageIndex];

        vk::Result fenceWait = m_logicalDevice.waitForFences(
            1,
            &currentImage.renderFence,
            VK_TRUE,
            0
        );

        if (fenceWait != vk::Result::eSuccess && fenceWait != vk::Result::eTimeout)
        {
            throw std::runtime_error("Error while waiting the fences");
        }

        if (
            m_logicalDevice.resetFences(
                1,
                &currentImage.renderFence
            ) != vk::Result::eSuccess
        )
        {
            throw std::runtime_error("Error while resetting the fences");
        }

        vk::ResultValue<uint32_t> acquireResult = m_logicalDevice.acquireNextImageKHR(
            m_swapChain.instance,
            UINT64_MAX,
            currentImage.presentSemaphore,
            nullptr
        );

        if (acquireResult.result == vk::Result::eErrorOutOfDateKHR)
        {
            rebuildSwapChain();
    
            return;
        }
    
        if (acquireResult.result != vk::Result::eSuccess && acquireResult.result != vk::Result::eSuboptimalKHR)
        {
            throw std::runtime_error("Failed to acquire swap chain image");
        }
        
        uint32_t imageIndex = acquireResult.value;

        std::vector<vk::CommandBuffer> commandBuffers;

        vk::CommandBuffer& commandBuffer = currentImage.commandBuffer;
        commandBuffers.push_back(commandBuffer);

        commandBuffer.reset();

        Frame::Instance& nextImage = m_swapChain.images[imageIndex];

        prepareCamera(nextImage);

        for (Layer* layer : m_layers)
        {
            layer->setup(nextImage);
        }

        nextImage.updateDescriptorSets();

        CommandBuffer::Worker::startJob(commandBuffer);

        for (Layer* layer : m_layers)
        {
            layer->render(
                nextImage,
                commandBuffer,
                m_swapChain.extent
            );
        }

        vk::Semaphore waitSemaphores[]      = { currentImage.presentSemaphore };
        vk::Semaphore signalSemaphores[]    = { currentImage.renderSemaphore };
        vk::PipelineStageFlags waitStages[] = { vk::PipelineStageFlagBits::eColorAttachmentOutput };
    
        vk::SubmitInfo submitInfo = {};
        submitInfo.waitSemaphoreCount   = 1;
        submitInfo.pWaitSemaphores      = waitSemaphores;
        submitInfo.commandBufferCount   = static_cast<uint32_t>(commandBuffers.size());
        submitInfo.pCommandBuffers      = commandBuffers.data();
        submitInfo.signalSemaphoreCount = 1;
        submitInfo.pSignalSemaphores    = signalSemaphores;
        submitInfo.pWaitDstStageMask    = waitStages;

        CommandBuffer::Worker::endJob(
            m_graphicsQueue,
            submitInfo,
            "Render submtion"
        );

        vk::SwapchainKHR swapChains[] = { m_swapChain.instance };

        vk::PresentInfoKHR presentInfo = {};
        presentInfo.waitSemaphoreCount = 1;
        presentInfo.pWaitSemaphores    = signalSemaphores;
        presentInfo.swapchainCount     = 1;
        presentInfo.pSwapchains        = swapChains;
        presentInfo.pImageIndices      = &imageIndex;

        vk::Result presentResult = m_presentQueue.presentKHR(presentInfo);

        if (
            presentResult == vk::Result::eErrorOutOfDateKHR ||
            presentResult == vk::Result::eSuboptimalKHR
        )
        {
            rebuildSwapChain();

            return;
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
        if (IS_DEBUGGING == false)
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
            m_window->getResolution()
        );

        m_camera->setResolution(
            m_swapChain.extent.width,
            m_swapChain.extent.height
        );

        m_imageCount = static_cast<int>(m_swapChain.images.size());

        for (Frame::Instance& frame : m_swapChain.images)
        {
            frame.setupSync();
        }
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
        for (Frame::Instance& frame : m_swapChain.images)
        {
            frame.destroy();
        }

        m_swapChain.images.clear();

        m_logicalDevice.destroySwapchainKHR(m_swapChain.instance);

        destroyLayers();
    }

    void Renderer::destroyLayers()
    {
        for (Layer* layer : m_layers)
        {
            layer->destroy();
        }
    }

    void Renderer::rebuildLayers()
    {
        for (Layer* layer : m_layers)
        {
            layer->rebuild();
        }
    }

    void Renderer::deleteLayers()
    {
        for (Layer* layer : m_layers)
        {
            delete layer;
        }

        m_layers.clear();
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
            m_swapChain.images,
            createInfo
        );
    }

    void Renderer::prepareCamera(Frame::Instance& outFrame)
    {
        outFrame.cameraVectorUBO.instance = m_camera->getVectorUBO();
        memcpy(
            outFrame.cameraVectorUBO.writeLocation,
            &outFrame.cameraVectorUBO.instance,
            outFrame.cameraVectorUBO.allocationSize
        );

        outFrame.cameraMatrixUBO.instance = m_camera->getMatrixUBO();
        memcpy(
            outFrame.cameraMatrixUBO.writeLocation,
            &outFrame.cameraMatrixUBO.instance,
            outFrame.cameraMatrixUBO.allocationSize
        );
    }
}