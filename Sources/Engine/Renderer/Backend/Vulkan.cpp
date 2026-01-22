#include "Chicane/Renderer/Backend/Vulkan.hpp"

#include "Chicane/Renderer/Instance.hpp"
#include "Chicane/Renderer/Backend/Vulkan/CommandBuffer.hpp"
#include "Chicane/Renderer/Backend/Vulkan/CommandBuffer/Pool.hpp"
#include "Chicane/Renderer/Backend/Vulkan/Debug.hpp"
#include "Chicane/Renderer/Backend/Vulkan/Device.hpp"
#include "Chicane/Renderer/Backend/Vulkan/Queue.hpp"
#include "Chicane/Renderer/Backend/Vulkan/Instance.hpp"
#include "Chicane/Renderer/Backend/Vulkan/Surface.hpp"
#include "Chicane/Renderer/Backend/Vulkan/Swapchain.hpp"

namespace Chicane
{
    namespace Renderer
    {
        VulkanBackend::VulkanBackend(const Instance* inRenderer)
            : Backend(inRenderer),
              m_swapchain({}),
              m_imageCount(0),
              m_currentImageIndex(0),
              m_vkViewport({}),
              m_vkScissor(vk::Rect2D())
        {}

        VulkanBackend::~VulkanBackend()
        {
            m_logicalDevice.waitIdle();

            // Vulkan
            destroyCommandPool();
            destroySwapchain();
            //deleteLayers();

            destroyDevices();
            destroySurface();

            if (IS_DEBUGGING)
            {
                destroyDebugMessenger();
            }

            destroyInstance();
        }

        void VulkanBackend::onInit()
        {
            buildInstance();
            buildDebugMessenger();
            buildSurface();
            buildDevices();
            buildQueues();
            buildSwapchain();
            buildCommandPool();
            buildMainCommandBuffer();
            buildFramesCommandBuffers();
            //buildLayers();
        }

        void VulkanBackend::onResize(const Viewport& inViewport)
        {
            // Viewport
            m_vkViewport.width  = inViewport.size.x;
            m_vkViewport.height = inViewport.size.y;

            m_vkViewport.x = inViewport.position.x;
            m_vkViewport.y = inViewport.position.y;

            // Scissor
            m_vkScissor.extent.width  = inViewport.size.x;
            m_vkScissor.extent.height = inViewport.size.y;
        }

        void VulkanBackend::onRender(const Frame& inFrame)
        {
            VulkanFrame&       currentImage         = m_swapchain.frames.at(m_currentImageIndex);
            vk::CommandBuffer& currentCommandBuffer = currentImage.commandBuffer;

            currentImage.wait(m_logicalDevice);

            vk::ResultValue<std::uint32_t> acquireResult = m_logicalDevice.acquireNextImageKHR(
                m_swapchain.instance,
                UINT64_MAX,
                currentImage.presentSemaphore,
                nullptr
            );

            if (acquireResult.result == vk::Result::eErrorOutOfDateKHR)
            {
                rebuildSwapchain();

                return;
            }
            else if (acquireResult.result != vk::Result::eSuccess &&
                     acquireResult.result != vk::Result::eSuboptimalKHR)
            {
                throw std::runtime_error("Error while acquiring the next image");
            }

            currentImage.reset(m_logicalDevice);

            std::uint32_t nextImageIndex = acquireResult.value;
            VulkanFrame&  nextImage      = m_swapchain.frames.at(nextImageIndex);

            vk::CommandBufferBeginInfo commandBufferBegin = {};
            commandBufferBegin.flags                      = vk::CommandBufferUsageFlagBits::eOneTimeSubmit;
            currentCommandBuffer.reset();

            setupFrame(nextImage);

            nextImage.updateDescriptorSets();

            currentCommandBuffer.begin(commandBufferBegin);
            renderViewport(currentCommandBuffer);
            //renderLayers(nextImage, currentCommandBuffer);
            currentCommandBuffer.end();

            vk::PipelineStageFlags waitStages[] = {vk::PipelineStageFlagBits::eColorAttachmentOutput};

            vk::SubmitInfo submitInfo       = {};
            submitInfo.waitSemaphoreCount   = 1;
            submitInfo.pWaitSemaphores      = &currentImage.presentSemaphore;
            submitInfo.commandBufferCount   = 1;
            submitInfo.pCommandBuffers      = &currentCommandBuffer;
            submitInfo.signalSemaphoreCount = 1;
            submitInfo.pSignalSemaphores    = &currentImage.renderSemaphore;
            submitInfo.pWaitDstStageMask    = waitStages;

            vk::Result submitResult = m_graphicsQueue.submit(1, &submitInfo, currentImage.renderFence);

            if (submitResult != vk::Result::eSuccess)
            {
                throw std::runtime_error("Error while submiting the next image");
            }

            vk::PresentInfoKHR presentInfo = {};
            presentInfo.waitSemaphoreCount = 1;
            presentInfo.pWaitSemaphores    = &currentImage.renderSemaphore;
            presentInfo.swapchainCount     = 1;
            presentInfo.pSwapchains        = &m_swapchain.instance;
            presentInfo.pImageIndices      = &nextImageIndex;

            vk::Result presentResult = m_presentQueue.presentKHR(presentInfo);

            if (presentResult == vk::Result::eErrorOutOfDateKHR ||
                presentResult == vk::Result::eSuboptimalKHR)
            {
                rebuildSwapchain();
            }
            else if (presentResult != vk::Result::eSuccess)
            {
                throw std::runtime_error("Error while presenting the image");
            }

            m_currentImageIndex = (m_currentImageIndex + 1) % m_imageCount;
        }

        void VulkanBackend::onHandle(const WindowEvent& inEvent)
        {
            if (inEvent.type == WindowEventType::WindowResized)
            {
                rebuildSwapchain();
            }
        }

        void VulkanBackend::buildInstance()
        {
            VulkanInstance::init(m_instance, m_dispatcher);
        }

        void VulkanBackend::destroyInstance()
        {
            m_instance.destroy();
        }

        void VulkanBackend::buildDebugMessenger()
        {
            VulkanDebug::initMessenger(m_debugMessenger, m_instance, m_dispatcher);
        }

        void VulkanBackend::destroyDebugMessenger()
        {
            VulkanDebug::destroyMessenger(m_debugMessenger, m_instance, m_dispatcher);
        }

        void VulkanBackend::buildSurface()
        {
            VulkanSurface::init(m_surface, m_instance, m_renderer->getWindow()->getInstance());
        }

        void VulkanBackend::destroySurface()
        {
            m_instance.destroySurfaceKHR(m_surface);
        }

        void VulkanBackend::buildQueues()
        {
            VulkanQueue::initGraphicsQueue(m_graphicsQueue, m_physicalDevice, m_logicalDevice, m_surface);
            VulkanQueue::initPresentQueue(m_presentQueue, m_physicalDevice, m_logicalDevice, m_surface);
        }

        void VulkanBackend::buildDevices()
        {
            VulkanDevice::pickPhysicalDevice(m_physicalDevice, m_instance);
            VulkanDevice::initLogicalDevice(m_logicalDevice, m_physicalDevice, m_surface);
        }

        void VulkanBackend::destroyDevices()
        {
            m_logicalDevice.destroy();
        }

        void VulkanBackend::buildSwapchain()
        {
            VulkanSwapchain::init(m_swapchain, m_physicalDevice, m_logicalDevice, m_surface);

            m_imageCount = static_cast<int>(m_swapchain.frames.size());

            for (VulkanFrame& frame : m_swapchain.frames)
            {
                // Images
                frame.setupColorImage(m_swapchain.colorFormat, m_swapchain.extent);
                frame.setupDepthImage(m_swapchain.depthFormat, m_swapchain.extent);
                frame.setupShadowImage(m_swapchain.depthFormat, m_swapchain.extent);

                // Sync
                frame.setupSync();
            }
        }

        void VulkanBackend::destroySwapchain()
        {
            for (VulkanFrame& frame : m_swapchain.frames)
            {
                frame.destroy();
            }

            m_swapchain.frames.clear();

            m_logicalDevice.destroySwapchainKHR(m_swapchain.instance);

            //destroyLayers();
        }

        void VulkanBackend::rebuildSwapchain()
        {
            if (m_renderer->getWindow()->isMinimized())
            {
                return;
            }

            m_logicalDevice.waitIdle();

            destroySwapchain();
            buildSwapchain();

            buildFramesCommandBuffers();

            rebuildFrames();
            //rebuildLayers();
        }

        void VulkanBackend::buildCommandPool()
        {
            VulkanCommandBufferPool::init(m_mainCommandPool, m_logicalDevice, m_physicalDevice, m_surface);
        }

        void VulkanBackend::destroyCommandPool()
        {
            m_logicalDevice.destroyCommandPool(m_mainCommandPool);
        }

        void VulkanBackend::buildMainCommandBuffer()
        {
            VulkanCommandBufferCreateInfo createInfo = {m_logicalDevice, m_mainCommandPool};
            VulkanCommandBuffer::init(m_mainCommandBuffer, createInfo);
        }

        void VulkanBackend::buildFramesCommandBuffers()
        {
            VulkanCommandBufferCreateInfo createInfo = {m_logicalDevice, m_mainCommandPool};

            for (VulkanFrame& frame : m_swapchain.frames)
            {
                VulkanCommandBuffer::init(frame.commandBuffer, createInfo);
            }
        }

        void VulkanBackend::renderViewport(const vk::CommandBuffer& inCommandBuffer)
        {
            inCommandBuffer.setViewport(0, 1, &m_vkViewport);
            inCommandBuffer.setScissor(0, 1, &m_vkScissor);
        }

        void VulkanBackend::rebuildFrames()
        {
            for (VulkanFrame& frame : m_swapchain.frames)
            {
                setupFrame(frame);
            }
        }

        void VulkanBackend::setupFrame(VulkanFrame& outFrame)
        {}
    }
}