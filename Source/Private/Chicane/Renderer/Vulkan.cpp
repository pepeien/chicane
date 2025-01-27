#include "Chicane/Renderer/Vulkan.hpp"

#include "Chicane/Application.hpp"
#include "Chicane/Core.hpp"
#include "Chicane/Game.hpp"
#include "Chicane/Renderer/Vulkan/Layer/LevelLayer.hpp"
#include "Chicane/Renderer/Vulkan/Layer/UILayer.hpp"
#include "Chicane/Renderer/Vulkan/Layer/ShadowLayer.hpp"
#include "Chicane/Renderer/Vulkan/Layer/SkyboxLayer.hpp"

namespace Chicane
{
    namespace Vulkan
    {
        Renderer::Renderer(Window::Instance* inWindow)
            : Chicane::Renderer::Instance(inWindow),
            m_swapChain({}),
            m_imageCount(0),
            m_currentImageIndex(0)
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

            loadEvents();
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

        void Renderer::initLayers()
        {
            pushLayer(new SkyboxLayer());
            pushLayer(new ShadowLayer());
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

            setupFrame(currentImage);
            setupLayers(currentImage);

            std::uint32_t nextImageIndex = acquireResult.value;
            Frame::Instance& nextImage   = m_swapChain.frames.at(nextImageIndex);

            vk::CommandBufferBeginInfo commandBufferBegin {};
            commandBufferBegin.flags = vk::CommandBufferUsageFlagBits::eOneTimeSubmit;
            currentCommandBuffer.reset();

            nextImage.updateDescriptorSets();

            currentCommandBuffer.begin(commandBufferBegin);
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

        void Renderer::loadEvents()
        {
            Application::watchLevel(
                [this](Level* inLevel) {
                    if (!inLevel)
                    {
                        return;
                    }

                    inLevel->watchComponents(
                        [this](const std::vector<Component*>& inComponents) {
                            m_cameras.clear();
                            m_lights.clear();
                            m_meshes.clear();

                            const Chicane::Renderer::Viewport& viewport = getViewport();

                            for (Component* component : inComponents)
                            {
                                if (component->isType<CCamera>())
                                {
                                    m_cameras.push_back(static_cast<CCamera*>(component));
                                    m_cameras.back()->setViewport(viewport.size);
                                }

                                if (component->isType<CLight>())
                                {
                                    m_lights.push_back(static_cast<CLight*>(component));
                                    m_lights.back()->setViewport(viewport.size);
                                }

                                if (component->isType<CMesh>())
                                {
                                    m_meshes.push_back(static_cast<CMesh*>(component));
                                }
                            }

                            std::sort(
                                m_meshes.begin(),
                                m_meshes.end(),
                                [](CMesh* inA, CMesh* inB)
                                {
                                    return strcmp(inA->getModel().c_str(), inB->getModel().c_str()) > 0;
                                }
                            );
                        }
                    );
                }
            );
        }

        void Renderer::buildInstance()
        {
            Vulkan::Instance::init(
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

            m_viewport.size.x = m_swapChain.extent.width;
            m_viewport.size.y = m_swapChain.extent.height;
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

        void Renderer::renderViewport(const vk::CommandBuffer& inCommandBuffer)
        {
            vk::Viewport viewport = GraphicsPipeline::createViewport(
                m_viewport.size,
                m_viewport.position
            );
            inCommandBuffer.setViewport(0, 1, &viewport);

            vk::Rect2D scissor = GraphicsPipeline::createScissor(m_viewport.size);
            inCommandBuffer.setScissor(0, 1, &scissor);
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

        void Renderer::setupLayers(Frame::Instance& outFrame)
        {
            for (Layer::Instance* layer : m_layers)
            {
                layer->setup(&outFrame);
            }
        }

        void Renderer::renderLayers(Frame::Instance& outFrame, const vk::CommandBuffer& inCommandBuffer)
        {
            Data data = {};
            data.commandBuffer   = inCommandBuffer;
            data.frame           = outFrame;
            data.swapChainExtent = m_swapChain.extent;

            for (Layer::Instance* layer : m_layers)
            {
                layer->render(&data);
            }
        }
    }
}