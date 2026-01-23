#include "Chicane/Renderer/Backend/Vulkan.hpp"

#include "Chicane/Renderer/Instance.hpp"
#include "Chicane/Renderer/Backend/Vulkan/CommandBuffer.hpp"
#include "Chicane/Renderer/Backend/Vulkan/CommandBuffer/Pool.hpp"
#include "Chicane/Renderer/Backend/Vulkan/Debug.hpp"
#include "Chicane/Renderer/Backend/Vulkan/Device.hpp"
#include "Chicane/Renderer/Backend/Vulkan/Queue.hpp"
#include "Chicane/Renderer/Backend/Vulkan/Instance.hpp"
#include "Chicane/Renderer/Backend/Vulkan/Layer/Grid.hpp"
#include "Chicane/Renderer/Backend/Vulkan/Layer/Scene.hpp"
#include "Chicane/Renderer/Backend/Vulkan/Surface.hpp"
#include "Chicane/Renderer/Backend/Vulkan/Swapchain.hpp"

namespace Chicane
{
    namespace Renderer
    {
        VulkanBackend::VulkanBackend(const Instance* inRenderer)
            : Backend(inRenderer),
              swapchain({}),
              imageCount(0),
              m_currentImageIndex(0),
              m_vkViewport({}),
              m_vkScissor(vk::Rect2D())
        {}

        VulkanBackend::~VulkanBackend()
        {
            logicalDevice.waitIdle();

            // Vulkan
            destroyCommandPool();
            destroySwapchain();
            deleteLayers();

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
            buildLayers();

            Backend::onInit();
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

            Backend::onResize(inViewport);
        }

        void VulkanBackend::onRender(const Frame& inFrame)
        {
            VulkanFrame& lastFrame = swapchain.frames.at(m_currentImageIndex);
            lastFrame.wait(logicalDevice);

            vk::ResultValue<std::uint32_t> acquireResult =
                logicalDevice
                    .acquireNextImageKHR(swapchain.instance, UINT64_MAX, lastFrame.presentSemaphore, nullptr);

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

            lastFrame.reset(logicalDevice);

            std::uint32_t frameIndex = acquireResult.value;
            VulkanFrame&  frame      = swapchain.frames.at(frameIndex);

            frame.updateCameraData(inFrame.getCamera());
            frame.updateLightData(inFrame.getLights());
            frame.update2DData(inFrame.getInstances2D());
            frame.update3DData(inFrame.getInstances3D());
            frame.updateDescriptorSets();

            vk::CommandBufferBeginInfo commandBufferBegin;
            commandBufferBegin.flags = vk::CommandBufferUsageFlagBits::eOneTimeSubmit;

            VulkanBackendData data;
            data.frame         = frame;
            data.commandBuffer = lastFrame.commandBuffer;

            lastFrame.commandBuffer.reset();
            lastFrame.commandBuffer.begin(commandBufferBegin);
            renderViewport(data.commandBuffer);
            renderLayers(inFrame, &data);
            lastFrame.commandBuffer.end();

            vk::PipelineStageFlags waitStages[] = {vk::PipelineStageFlagBits::eColorAttachmentOutput};

            vk::SubmitInfo submitInfo       = {};
            submitInfo.waitSemaphoreCount   = 1;
            submitInfo.pWaitSemaphores      = &lastFrame.presentSemaphore;
            submitInfo.commandBufferCount   = 1;
            submitInfo.pCommandBuffers      = &lastFrame.commandBuffer;
            submitInfo.signalSemaphoreCount = 1;
            submitInfo.pSignalSemaphores    = &lastFrame.renderSemaphore;
            submitInfo.pWaitDstStageMask    = waitStages;

            vk::Result submitResult = graphicsQueue.submit(1, &submitInfo, lastFrame.renderFence);

            if (submitResult != vk::Result::eSuccess)
            {
                throw std::runtime_error("Error while submiting the next image");
            }

            vk::PresentInfoKHR presentInfo = {};
            presentInfo.waitSemaphoreCount = 1;
            presentInfo.pWaitSemaphores    = &lastFrame.renderSemaphore;
            presentInfo.swapchainCount     = 1;
            presentInfo.pSwapchains        = &swapchain.instance;
            presentInfo.pImageIndices      = &frameIndex;

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

            m_currentImageIndex = (m_currentImageIndex + 1) % imageCount;
        }

        void VulkanBackend::onHandle(const WindowEvent& inEvent)
        {
            if (inEvent.type == WindowEventType::WindowResized)
            {
                rebuildSwapchain();
            }

            Backend::onHandle(inEvent);
        }

        void VulkanBackend::buildInstance()
        {
            VulkanInstance::init(instance, m_dispatcher);
        }

        void VulkanBackend::destroyInstance()
        {
            instance.destroy();
        }

        void VulkanBackend::buildDebugMessenger()
        {
            VulkanDebug::initMessenger(m_debugMessenger, instance, m_dispatcher);
        }

        void VulkanBackend::destroyDebugMessenger()
        {
            VulkanDebug::destroyMessenger(m_debugMessenger, instance, m_dispatcher);
        }

        void VulkanBackend::buildSurface()
        {
            VulkanSurface::init(surface, instance, m_renderer->getWindow()->getInstance());
        }

        void VulkanBackend::destroySurface()
        {
            instance.destroySurfaceKHR(surface);
        }

        void VulkanBackend::buildQueues()
        {
            VulkanQueue::initGraphicsQueue(graphicsQueue, physicalDevice, logicalDevice, surface);
            VulkanQueue::initPresentQueue(m_presentQueue, physicalDevice, logicalDevice, surface);
        }

        void VulkanBackend::buildDevices()
        {
            VulkanDevice::pickPhysicalDevice(physicalDevice, instance);
            VulkanDevice::initLogicalDevice(logicalDevice, physicalDevice, surface);
        }

        void VulkanBackend::destroyDevices()
        {
            logicalDevice.destroy();
        }

        void VulkanBackend::buildSwapchain()
        {
            VulkanSwapchain::init(swapchain, physicalDevice, logicalDevice, surface);

            imageCount = static_cast<int>(swapchain.frames.size());

            for (VulkanFrame& frame : swapchain.frames)
            {
                // Images
                frame.setupColorImage(swapchain.colorFormat, swapchain.extent);
                frame.setupDepthImage(swapchain.depthFormat, swapchain.extent);
                frame.setupShadowImage(swapchain.depthFormat, swapchain.extent);

                // Sync
                frame.setupSync();
            }
        }

        void VulkanBackend::destroySwapchain()
        {
            for (VulkanFrame& frame : swapchain.frames)
            {
                frame.destroy();
            }

            swapchain.frames.clear();

            logicalDevice.destroySwapchainKHR(swapchain.instance);

            destroyLayers();
        }

        void VulkanBackend::rebuildSwapchain()
        {
            if (m_renderer->getWindow()->isMinimized())
            {
                return;
            }

            logicalDevice.waitIdle();

            destroySwapchain();
            buildSwapchain();

            buildFramesCommandBuffers();

            rebuildLayers();
        }

        void VulkanBackend::buildCommandPool()
        {
            VulkanCommandBufferPool::init(m_mainCommandPool, logicalDevice, physicalDevice, surface);
        }

        void VulkanBackend::destroyCommandPool()
        {
            logicalDevice.destroyCommandPool(m_mainCommandPool);
        }

        void VulkanBackend::buildMainCommandBuffer()
        {
            VulkanCommandBufferCreateInfo createInfo = {logicalDevice, m_mainCommandPool};
            VulkanCommandBuffer::init(mainCommandBuffer, createInfo);
        }

        void VulkanBackend::buildFramesCommandBuffers()
        {
            VulkanCommandBufferCreateInfo createInfo = {logicalDevice, m_mainCommandPool};

            for (VulkanFrame& frame : swapchain.frames)
            {
                VulkanCommandBuffer::init(frame.commandBuffer, createInfo);
            }
        }

        void VulkanBackend::renderViewport(const vk::CommandBuffer& inCommandBuffer)
        {
            inCommandBuffer.setViewport(0, 1, &m_vkViewport);
            inCommandBuffer.setScissor(0, 1, &m_vkScissor);
        }

        void VulkanBackend::buildLayers()
        {
            addLayer<VulkanLScene>();
            addLayer<VulkanLGrid>();
        }

        void VulkanBackend::renderLayers(const Frame& inFrame, void* inData)
        {
            for (Layer* layer : m_layers)
            {
                layer->render(inFrame, inData);
            }
        }
    }
}