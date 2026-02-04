#include "Chicane/Renderer/Backend/Vulkan.hpp"

#include "Chicane/Renderer/Instance.hpp"
#include "Chicane/Renderer/Backend/Vulkan/CommandBuffer.hpp"
#include "Chicane/Renderer/Backend/Vulkan/CommandBuffer/Pool.hpp"
#include "Chicane/Renderer/Backend/Vulkan/Debug.hpp"
#include "Chicane/Renderer/Backend/Vulkan/Descriptor/Pool.hpp"
#include "Chicane/Renderer/Backend/Vulkan/Descriptor/SetLayout.hpp"
#include "Chicane/Renderer/Backend/Vulkan/Descriptor/SetLayout/BidingsCreateInfo.hpp"
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
        VulkanBackend::VulkanBackend()
            : Backend<Frame>(),
              swapchain({}),
              imageCount(0),
              m_currentImageIndex(0),
              viewport({}),
              scissor(vk::Rect2D())
        {}

        VulkanBackend::~VulkanBackend()
        {
            logicalDevice.waitIdle();

            // Vulkan
            destroyCommandPool();
            destroySwapchain();
            destroyTextureData();
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
            setFrameCount(2);

            buildInstance();
            buildDebugMessenger();
            buildSurface();
            buildDevices();
            buildQueues();
            buildSwapchain();
            buildCommandPool();
            buildMainCommandBuffer();
            buildFramesCommandBuffers();
            buildTextureDescriptor();
            buildLayers();

            Backend::onInit();
        }

        void VulkanBackend::onResize(const Vec<2, std::uint32_t>& inResolution)
        {
            viewport.x        = 0.0f;
            viewport.y        = 0.0f;
            viewport.width    = inResolution.x;
            viewport.height   = inResolution.y;
            viewport.minDepth = 0.0f;
            viewport.maxDepth = 1.0f;

            scissor.offset.x      = 0U;
            scissor.offset.y      = 0U;
            scissor.extent.width  = static_cast<uint32_t>(viewport.width);
            scissor.extent.height = static_cast<uint32_t>(viewport.height);

            Backend::onResize(inResolution);
        }

        void VulkanBackend::onLoad(const DrawTexture::List& inResources)
        {
            buildTextureData(inResources);
        }

        void VulkanBackend::onRender()
        {
            VulkanFrame& lastFrame = swapchain.frames.at(m_currentImageIndex);
            lastFrame.wait(logicalDevice);

            vk::ResultValue<std::uint32_t> acquireResult =
                logicalDevice.acquireNextImageKHR(swapchain.instance, UINT64_MAX, lastFrame.presentSemaphore, nullptr);

            if (acquireResult.result == vk::Result::eErrorOutOfDateKHR)
            {
                rebuildSwapchain();

                return;
            }
            else if (acquireResult.result != vk::Result::eSuccess && acquireResult.result != vk::Result::eSuboptimalKHR)
            {
                throw std::runtime_error("Error while acquiring the next image");
            }

            lastFrame.reset(logicalDevice);

            std::uint32_t frameIndex = acquireResult.value;
            VulkanFrame&  frame      = swapchain.frames.at(frameIndex);

            frame.updateCameraData(getCurrentFrame().getCamera());
            frame.updateLightData(getCurrentFrame().getLights());
            frame.update2DData(getCurrentFrame().getInstances2D());
            frame.update3DData(getCurrentFrame().getInstances3D());
            frame.updateDescriptorSets();

            vk::CommandBufferBeginInfo commandBufferBegin;
            commandBufferBegin.flags = vk::CommandBufferUsageFlagBits::eOneTimeSubmit;

            VulkanBackendData data;
            data.frame         = frame;
            data.commandBuffer = lastFrame.commandBuffer;

            lastFrame.commandBuffer.reset();
            lastFrame.commandBuffer.begin(commandBufferBegin);
            renderViewport(data.commandBuffer);
            renderLayers(&data);
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

            if (presentResult == vk::Result::eErrorOutOfDateKHR || presentResult == vk::Result::eSuboptimalKHR)
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
            VulkanSurface::init(surface, instance, m_window->getInstance());
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
            if (m_window->isMinimized())
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
            inCommandBuffer.setViewport(0, 1, &viewport);
            inCommandBuffer.setScissor(0, 1, &scissor);
        }

        void VulkanBackend::buildLayers()
        {
            ListPush<Layer<Frame>*> settings;

            settings.strategy = ListPushStrategy::Front;
            addLayer<VulkanLScene>(settings);

            settings.strategy = ListPushStrategy::Back;
            addLayer<VulkanLGrid>(settings);
        }

        void VulkanBackend::renderLayers(void* inData)
        {
            for (Layer<Frame>* layer : m_layers)
            {
                layer->render(getCurrentFrame(), inData);
            }
        }

        void VulkanBackend::buildTextureDescriptor()
        {
            VulkanDescriptorSetLayoutBidingsCreateInfo layoutBidings;
            layoutBidings.count = 1;

            layoutBidings.indices.push_back(0);
            layoutBidings.types.push_back(vk::DescriptorType::eCombinedImageSampler);
            layoutBidings.counts.push_back(512);
            layoutBidings.stages.push_back(vk::ShaderStageFlagBits::eFragment);

            VulkanDescriptorSetLayout::init(textureDescriptor.setLayout, logicalDevice, layoutBidings);

            VulkanDescriptorPoolCreateInfo descriptorPoolCreateInfo;
            descriptorPoolCreateInfo.maxSets = 1;
            descriptorPoolCreateInfo.sizes.push_back({vk::DescriptorType::eCombinedImageSampler, 512});

            VulkanDescriptorPool::init(textureDescriptor.pool, logicalDevice, descriptorPoolCreateInfo);

            VulkanDescriptorSetLayout::allocate(
                textureDescriptor.set,
                logicalDevice,
                textureDescriptor.setLayout,
                textureDescriptor.pool
            );
        }

        void VulkanBackend::buildTextureData(const DrawTexture::List& inTextures)
        {
            if (inTextures.empty())
            {
                return;
            }

            VulkanTextureCreateInfo createInfo;
            createInfo.logicalDevice  = logicalDevice;
            createInfo.physicalDevice = physicalDevice;
            createInfo.commandBuffer  = mainCommandBuffer;
            createInfo.queue          = graphicsQueue;

            std::vector<vk::DescriptorImageInfo> infos = {};
            for (const DrawTexture& texture : inTextures)
            {
                createInfo.image = texture.image;
                textures.push_back(std::make_unique<VulkanTexture>(createInfo));

                vk::DescriptorImageInfo info;
                info.imageLayout = vk::ImageLayout::eShaderReadOnlyOptimal;
                info.imageView   = textures.back()->getImage().view;
                info.sampler     = textures.back()->getImage().sampler;
                infos.push_back(info);
            }

            vk::WriteDescriptorSet set;
            set.dstSet          = textureDescriptor.set;
            set.dstBinding      = 0;
            set.dstArrayElement = 0;
            set.descriptorCount = static_cast<std::uint32_t>(infos.size());
            set.descriptorType  = vk::DescriptorType::eCombinedImageSampler;
            set.pImageInfo      = infos.data();

            logicalDevice.updateDescriptorSets(set, nullptr);
        }

        void VulkanBackend::destroyTextureData()
        {
            textures.clear();

            logicalDevice.destroyDescriptorSetLayout(textureDescriptor.setLayout);
            logicalDevice.destroyDescriptorPool(textureDescriptor.pool);
        }
    }
}