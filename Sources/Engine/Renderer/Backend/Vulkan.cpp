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
#include "Chicane/Renderer/Backend/Vulkan/Layer/Scene.hpp"
#include "Chicane/Renderer/Backend/Vulkan/Layer/UI.hpp"
#include "Chicane/Renderer/Backend/Vulkan/Surface.hpp"
#include "Chicane/Renderer/Backend/Vulkan/Swapchain.hpp"

namespace Chicane
{
    namespace Renderer
    {
        VulkanBackend::VulkanBackend()
            : Backend(),
              swapchain({}),
              frames({}),
              m_currentFrameIndex(0U)
        {}

        VulkanBackend::~VulkanBackend()
        {
            onShutdown();
        }

        void VulkanBackend::onInit()
        {
            buildInstance();
            buildDebugMessenger();
            buildSurface();
            buildDevices();
            updateResourceBudget();
            buildQueues();
            buildCommandPool();
            buildMainCommandBuffer();
            buildSwapchain();
            buildFrames();
            buildTextureDescriptor();
            buildLayers();
        }

        void VulkanBackend::onShutdown()
        {
            logicalDevice.waitIdle();

            // Vulkan
            destroyCommandPool();
            destroySwapchain();
            destroyFrames();
            destroyTextureData();
            destroyLayers();

            destroyDevices();
            destroySurface();

            if (IS_DEBUGGING)
            {
                destroyDebugMessenger();
            }

            destroyInstance();
        }

        void VulkanBackend::onResize()
        {
            rebuildSwapchain();

            Backend::onResize();
        }

        void VulkanBackend::onLoad(const DrawTextureResource& inResources)
        {
            buildTextureData(inResources.getDraws());

            Backend::onLoad(inResources);
        }

        void VulkanBackend::onRender(const Frame& inFrame)
        {
            VulkanFrame& nextFrame = frames.at(m_currentFrameIndex);
            nextFrame.wait();

            const auto [result, imageIndex] =
                logicalDevice
                    .acquireNextImageKHR(swapchain.instance, UINT64_MAX, nextFrame.imageAvailableSemaphore, nullptr);
            if (result == vk::Result::eErrorOutOfDateKHR)
            {
                rebuildSwapchain();

                return;
            }
            else if (result != vk::Result::eSuccess && result != vk::Result::eSuboptimalKHR)
            {
                throw std::runtime_error("Error while acquiring the next image");
            }

            VulkanSwapchainImage& nextImage = swapchain.images.at(imageIndex);

            nextFrame.begin(inFrame, nextImage);
            renderLayers(inFrame, &nextFrame);
            nextFrame.end();

            vk::PipelineStageFlags waitStages[] = {vk::PipelineStageFlagBits::eColorAttachmentOutput};

            vk::SubmitInfo submitInfo;
            submitInfo.waitSemaphoreCount   = 1;
            submitInfo.pWaitSemaphores      = &nextFrame.imageAvailableSemaphore;
            submitInfo.pWaitDstStageMask    = waitStages;
            submitInfo.commandBufferCount   = 1;
            submitInfo.pCommandBuffers      = &nextFrame.commandBuffer;
            submitInfo.signalSemaphoreCount = 1;
            submitInfo.pSignalSemaphores    = &nextImage.renderFineshedSemaphore;

            vk::Result submitResult = graphicsQueue.submit(1, &submitInfo, nextFrame.fence);
            if (submitResult != vk::Result::eSuccess)
            {
                throw std::runtime_error("Queue submit failed");
            }

            vk::PresentInfoKHR presentInfo;
            presentInfo.waitSemaphoreCount = 1;
            presentInfo.pWaitSemaphores    = &nextImage.renderFineshedSemaphore;
            presentInfo.swapchainCount     = 1;
            presentInfo.pSwapchains        = &swapchain.instance;
            presentInfo.pImageIndices      = &imageIndex;

            vk::Result presentResult = m_presentQueue.presentKHR(presentInfo);
            if (presentResult == vk::Result::eErrorOutOfDateKHR || presentResult == vk::Result::eSuboptimalKHR)
            {
                rebuildSwapchain();

                return;
            }
            else if (presentResult != vk::Result::eSuccess)
            {
                throw std::runtime_error("Present failed");
            }

            m_currentFrameIndex = (m_currentFrameIndex + 1) % frames.size();
        }

        vk::Viewport VulkanBackend::getVkViewport(Layer* inLayer) const
        {
            Vec<2, std::uint32_t>   resolution = {swapchain.extent.width, swapchain.extent.height};
            const ViewportSettings& viewport   = inLayer->getViewport();

            Size size;
            size.setIsAsobute(true);
            size.setRoot(resolution);
            size.setParent(resolution);

            vk::Viewport result;
            result.x        = size.parse(viewport.offsetX, SizeDirection::Horizontal);
            result.y        = size.parse(viewport.offsetY, SizeDirection::Vertical);
            result.width    = size.parse(viewport.width, SizeDirection::Horizontal);
            result.height   = size.parse(viewport.height, SizeDirection::Vertical);
            result.minDepth = 0.0f;
            result.maxDepth = 1.0f;

            return result;
        }

        vk::Rect2D VulkanBackend::getVkScissor(Layer* inLayer) const
        {
            const Viewport viewport = getLayerViewport(inLayer);

            vk::Rect2D result;
            result.offset.x      = 0.0f;
            result.offset.y      = 0.0f;
            result.extent.width  = viewport.size.x;
            result.extent.height = viewport.size.y;

            return result;
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
            VulkanSurface::init(surface, instance, getRenderer()->getWindow()->getInstance());
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

        void VulkanBackend::updateResourceBudget()
        {
            vk::PhysicalDeviceMemoryProperties properties = physicalDevice.getMemoryProperties();

            std::size_t VRAM = 0U;
            for (const auto& memoryHeap : properties.memoryHeaps)
            {
                if (memoryHeap.flags & vk::MemoryHeapFlagBits::eDeviceLocal)
                {
                    VRAM += memoryHeap.size;
                }
            }

            setVRAM(VRAM);
        }

        void VulkanBackend::destroyDevices()
        {
            logicalDevice.destroy();
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
        void VulkanBackend::buildSwapchain()
        {
            VulkanSwapchain::init(swapchain, physicalDevice, logicalDevice, surface);

            for (VulkanSwapchainImage& image : swapchain.images)
            {
                // Sync
                image.setupSync();

                // Images
                image.setupColorImage(swapchain.colorFormat, swapchain.extent);
                image.setupDepthImage(swapchain.depthFormat, swapchain.extent);
            }
        }

        void VulkanBackend::destroySwapchain()
        {
            logicalDevice.waitIdle();

            for (VulkanSwapchainImage& image : swapchain.images)
            {
                image.destroy();
            }

            swapchain.images.clear();

            logicalDevice.destroySwapchainKHR(swapchain.instance);

            shutdownLayers();
        }

        void VulkanBackend::rebuildSwapchain()
        {
            if (getRenderer()->getWindow()->isMinimized())
            {
                return;
            }

            destroySwapchain();
            buildSwapchain();

            rebuildLayers();
        }

        void VulkanBackend::buildFrames()
        {
            frames.resize(m_renderer->getFrameInFlighCount());

            for (VulkanFrame& frame : frames)
            {
                frame.logicalDevice  = logicalDevice;
                frame.physicalDevice = physicalDevice;

                // Commandbuffer
                frame.setupCommandBuffer(m_mainCommandPool);

                // Sync
                frame.setupSync();

                // Data
                frame.setupCameraData();
                frame.setupLightData();
                frame.setup2DData(getResourceBudget(Resource::UIInstances));
                frame.setup3DData(getResourceBudget(Resource::SceneInstances));
            }
        }

        void VulkanBackend::destroyFrames()
        {
            for (VulkanFrame& frame : frames)
            {
                frame.destroy();
            }

            frames.clear();
        }

        void VulkanBackend::buildLayers()
        {
            ListPush<Layer*> settings;

            settings.strategy = ListPushStrategy::Front;
            addLayer<VulkanLScene>(settings);

            settings.strategy = ListPushStrategy::Back;
            addLayer<VulkanLUI>(settings);
        }

        void VulkanBackend::buildTextureDescriptor()
        {
            VulkanDescriptorSetLayoutBidingsCreateInfo layoutBidings;
            layoutBidings.count = 1;

            layoutBidings.indices.push_back(0);
            layoutBidings.types.push_back(vk::DescriptorType::eCombinedImageSampler);
            layoutBidings.counts.push_back(TEXTURE_COUNT);
            layoutBidings.stages.push_back(vk::ShaderStageFlagBits::eFragment);

            VulkanDescriptorSetLayout::init(textureDescriptor.setLayout, logicalDevice, layoutBidings);

            VulkanDescriptorPoolCreateInfo descriptorPoolCreateInfo;
            descriptorPoolCreateInfo.maxSets = 1;
            descriptorPoolCreateInfo.sizes.push_back({vk::DescriptorType::eCombinedImageSampler, TEXTURE_COUNT});

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