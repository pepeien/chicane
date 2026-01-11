#include "Chicane/Runtime/Renderer/Renderer.hpp"

#include "Chicane/Runtime/Application.hpp"
#include "Chicane/Runtime/Renderer/Vulkan/CommandBuffer.hpp"
#include "Chicane/Runtime/Renderer/Vulkan/CommandBuffer/Pool.hpp"
#include "Chicane/Runtime/Renderer/Vulkan/Debug.hpp"
#include "Chicane/Runtime/Renderer/Vulkan/Device.hpp"
#include "Chicane/Runtime/Renderer/Vulkan/Frame/Buffer.hpp"
#include "Chicane/Runtime/Renderer/Vulkan/Instance.hpp"
#include "Chicane/Runtime/Renderer/Vulkan/Layer/Grid.hpp"
#include "Chicane/Runtime/Renderer/Vulkan/Layer/Scene.hpp"
#include "Chicane/Runtime/Renderer/Vulkan/Surface.hpp"
#include "Chicane/Runtime/Scene.hpp"

namespace Chicane
{
    namespace Vulkan
    {
        Renderer::Renderer()
            : Super(),
              m_swapChain({}),
              m_imageCount(0),
              m_currentImageIndex(0),
              m_vkViewport({}),
              m_vkScissor(vk::Rect2D())
        {
            m_vkViewport.minDepth = 0.0f;
            m_vkViewport.maxDepth = 1.0f;

            m_vkScissor.offset.x = 0;
            m_vkScissor.offset.y = 0;
        }

        Renderer::~Renderer()
        {
            m_logicalDevice.waitIdle();

            // Vulkan
            destroyCommandPool();
            destroySwapChain();
            deleteLayers();

            destroyDevices();
            destroySurface();

            if (IS_DEBUGGING)
            {
                destroyDebugMessenger();
            }

            destroyInstance();
        }

        RendererInternals Renderer::getInternals()
        {
            RendererInternals internals{};
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

        void Renderer::onInit()
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
            buildLayers();
        }

        void Renderer::onEvent(const WindowEvent& inEvent)
        {
            if (inEvent.type == WindowEventType::WindowResized)
            {
                rebuildSwapChain();
            }
        }

        void Renderer::onResizing()
        {
            // Viewport
            m_vkViewport.width  = m_size.x;
            m_vkViewport.height = m_size.y;

            // Scissor
            m_vkScissor.extent.width  = m_size.x;
            m_vkScissor.extent.height = m_size.y;
        }

        void Renderer::onRepositioning()
        {
            // Viewport
            m_vkViewport.x = m_position.x;
            m_vkViewport.y = m_position.y;
        }

        void Renderer::onRender()
        {
            Frame::Instance&   currentImage         = m_swapChain.frames.at(m_currentImageIndex);
            vk::CommandBuffer& currentCommandBuffer = currentImage.commandBuffer;

            currentImage.wait(m_logicalDevice);

            vk::ResultValue<std::uint32_t> acquireResult =
                m_logicalDevice
                    .acquireNextImageKHR(m_swapChain.instance, UINT64_MAX, currentImage.presentSemaphore, nullptr);

            if (acquireResult.result == vk::Result::eErrorOutOfDateKHR)
            {
                rebuildSwapChain();

                return;
            }
            else if (acquireResult.result != vk::Result::eSuccess && acquireResult.result != vk::Result::eSuboptimalKHR)
            {
                throw std::runtime_error("Error while acquiring the next image");
            }

            currentImage.reset(m_logicalDevice);

            std::uint32_t    nextImageIndex = acquireResult.value;
            Frame::Instance& nextImage      = m_swapChain.frames.at(nextImageIndex);

            vk::CommandBufferBeginInfo commandBufferBegin = {};
            commandBufferBegin.flags                      = vk::CommandBufferUsageFlagBits::eOneTimeSubmit;
            currentCommandBuffer.reset();

            setupFrame(nextImage);

            nextImage.updateDescriptorSets();

            currentCommandBuffer.begin(commandBufferBegin);
            renderViewport(currentCommandBuffer);
            renderLayers(nextImage, currentCommandBuffer);
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
            presentInfo.pSwapchains        = &m_swapChain.instance;
            presentInfo.pImageIndices      = &nextImageIndex;

            vk::Result presentResult = m_presentQueue.presentKHR(presentInfo);

            if (presentResult == vk::Result::eErrorOutOfDateKHR || presentResult == vk::Result::eSuboptimalKHR)
            {
                rebuildSwapChain();
            }
            else if (presentResult != vk::Result::eSuccess)
            {
                throw std::runtime_error("Error while presenting the image");
            }

            m_currentImageIndex = (m_currentImageIndex + 1) % m_imageCount;
        }

        void Renderer::buildInstance()
        {
            Instance::init(m_instance, m_dispatcher);
        }

        void Renderer::destroyInstance()
        {
            m_instance.destroy();
        }

        void Renderer::buildDebugMessenger()
        {
            Debug::initMessenger(m_debugMessenger, m_instance, m_dispatcher);
        }

        void Renderer::destroyDebugMessenger()
        {
            Debug::destroyMessenger(m_debugMessenger, m_instance, m_dispatcher);
        }

        void Renderer::buildSurface()
        {
            Surface::init(m_surface, m_instance, m_window->getInstance());
        }

        void Renderer::destroySurface()
        {
            m_instance.destroySurfaceKHR(m_surface);
        }

        void Renderer::buildQueues()
        {
            Queue::initGraphicsQueue(m_graphicsQueue, m_physicalDevice, m_logicalDevice, m_surface);
            Queue::initPresentQueue(m_presentQueue, m_physicalDevice, m_logicalDevice, m_surface);
        }

        void Renderer::buildDevices()
        {
            Device::pickPhysicalDevice(m_physicalDevice, m_instance);
            Device::initLogicalDevice(m_logicalDevice, m_physicalDevice, m_surface);
        }

        void Renderer::destroyDevices()
        {
            m_logicalDevice.destroy();
        }

        void Renderer::buildSwapChain()
        {
            SwapChain::init(m_swapChain, m_physicalDevice, m_logicalDevice, m_surface);

            m_imageCount = static_cast<int>(m_swapChain.frames.size());

            for (Frame::Instance& frame : m_swapChain.frames)
            {
                // Images
                frame.setupColorImage(m_swapChain.colorFormat, m_swapChain.extent);
                frame.setupDepthImage(m_swapChain.depthFormat, m_swapChain.extent);
                frame.setupShadowImage(m_swapChain.depthFormat, m_swapChain.extent);

                // Sync
                frame.setupSync();
            }

            setSize(m_swapChain.extent.width, m_swapChain.extent.height);
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

        void Renderer::rebuildSwapChain()
        {
            if (m_window->isMinimized())
            {
                return;
            }

            m_logicalDevice.waitIdle();

            destroySwapChain();
            buildSwapChain();

            buildFramesCommandBuffers();

            rebuildFrames();
            rebuildLayers();
        }

        void Renderer::buildCommandPool()
        {
            CommandBuffer::Pool::init(m_mainCommandPool, m_logicalDevice, m_physicalDevice, m_surface);
        }

        void Renderer::destroyCommandPool()
        {
            m_logicalDevice.destroyCommandPool(m_mainCommandPool);
        }

        void Renderer::buildMainCommandBuffer()
        {
            CommandBuffer::CreateInfo createInfo = {m_logicalDevice, m_mainCommandPool};

            CommandBuffer::init(m_mainCommandBuffer, createInfo);
        }

        void Renderer::buildFramesCommandBuffers()
        {
            CommandBuffer::CreateInfo createInfo = {m_logicalDevice, m_mainCommandPool};

            Frame::Buffer::initCommand(m_swapChain.frames, createInfo);
        }

        void Renderer::renderViewport(const vk::CommandBuffer& inCommandBuffer)
        {
            inCommandBuffer.setViewport(0, 1, &m_vkViewport);
            inCommandBuffer.setScissor(0, 1, &m_vkScissor);
        }

        void Renderer::rebuildFrames()
        {
            for (Frame::Instance& frame : m_swapChain.frames)
            {
                setupFrame(frame);
            }
        }

        void Renderer::setupFrame(Frame::Instance& outFrame)
        {
            outFrame.updateCameraData(m_cameras);
            outFrame.updateLightData(m_lights);
            outFrame.updateMeshData(m_meshes);
        }

        void Renderer::buildLayers()
        {
            pushLayer<LScene>();
            pushLayer<LGrid>();
        }

        void Renderer::renderLayers(Frame::Instance& outFrame, const vk::CommandBuffer& inCommandBuffer)
        {
            RendererData data    = {};
            data.commandBuffer   = inCommandBuffer;
            data.frame           = outFrame;
            data.swapChainExtent = m_swapChain.extent;

            for (RendererLayer* layer : m_layers)
            {
                if (!layer)
                {
                    continue;
                }

                layer->render(&data);
            }
        }
    }
}