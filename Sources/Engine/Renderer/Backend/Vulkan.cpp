#include "Chicane/Renderer/Backend/Vulkan.hpp"

#include <algorithm>

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
              m_currentFrameIndex(0U),
              m_screenTextureId(Draw::InvalidId),
              m_timestampQueryPool(nullptr),
              m_timestampPeriod(1.0f),
              m_bTimestampsEnabled(false),
              m_timestampSubmitted({})
        {}

        VulkanBackend::~VulkanBackend()
        {
            onShutdown();
        }

        void VulkanBackend::onInit()
        {
            if (isStatus(BackendStatus::Running))
            {
                return;
            }

            Backend::onInit();

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
            buildTimestampQueries();
            buildTextureDescriptor();
            buildLayers();
        }

        void VulkanBackend::onShutdown()
        {
            if (isStatus(BackendStatus::Shutdown))
            {
                return;
            }

            Backend::onShutdown();

            logicalDevice.waitIdle();

            // Vulkan
            destroyCommandPool();
            destroyTimestampQueries();
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
            resolveGpuTimestamp(m_currentFrameIndex);

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

            nextFrame.reset();

            VulkanSwapchainImage& nextImage = swapchain.images.at(imageIndex);

            // Update before any bind in this CB — view/sampler are stable for this
            // swapchain image; flushTarget only refreshes contents later.
            bindScreenTarget(nextImage.targetImage);

            nextFrame.begin(inFrame, nextImage);
            writeGpuTimestampStart(nextFrame.commandBuffer, m_currentFrameIndex);
            renderLayers(
                inFrame,
                &nextFrame,
                [](const Layer* inLayer) { return !inLayer->getId().equals(UI_LAYER_ID); }
            );
            nextFrame.flushTarget();
            renderLayers(
                inFrame,
                &nextFrame,
                [](const Layer* inLayer) { return inLayer->getId().equals(UI_LAYER_ID); }
            );
            writeGpuTimestampEnd(nextFrame.commandBuffer, m_currentFrameIndex);
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

            if (m_bTimestampsEnabled)
            {
                m_timestampSubmitted[m_currentFrameIndex] = true;
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

        Draw::Id VulkanBackend::getScreenTextureId() const
        {
            return m_screenTextureId;
        }

        vk::DescriptorSet VulkanBackend::getTextureDescriptorSet() const
        {
            if (textureDescriptorSets.empty())
            {
                return textureDescriptor.set;
            }

            return textureDescriptorSets.at(m_currentFrameIndex);
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
                image.setupTargetImage(swapchain.colorFormat, swapchain.extent);
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

        void VulkanBackend::buildTimestampQueries()
        {
            m_bTimestampsEnabled = false;
            m_timestampSubmitted.assign(frames.size(), false);

            if (frames.empty())
            {
                return;
            }

            VulkanQueueFamilyIndices familyIndices(physicalDevice, surface);
            if (!familyIndices.graphicsFamily.has_value())
            {
                return;
            }

            const std::vector<vk::QueueFamilyProperties> queueProperties = physicalDevice.getQueueFamilyProperties();
            const vk::QueueFamilyProperties&             graphicsProperties =
                queueProperties.at(familyIndices.graphicsFamily.value());

            if (graphicsProperties.timestampValidBits == 0)
            {
                return;
            }

            m_timestampPeriod = physicalDevice.getProperties().limits.timestampPeriod;

            vk::QueryPoolCreateInfo createInfo;
            createInfo.queryType  = vk::QueryType::eTimestamp;
            createInfo.queryCount = static_cast<std::uint32_t>(frames.size() * 2U);

            m_timestampQueryPool = logicalDevice.createQueryPool(createInfo);
            m_bTimestampsEnabled = true;
        }

        void VulkanBackend::destroyTimestampQueries()
        {
            if (m_timestampQueryPool)
            {
                logicalDevice.destroyQueryPool(m_timestampQueryPool);
                m_timestampQueryPool = nullptr;
            }

            m_bTimestampsEnabled = false;
            m_timestampSubmitted.clear();
        }

        void VulkanBackend::resolveGpuTimestamp(std::uint32_t inFrameIndex)
        {
            if (!m_bTimestampsEnabled || !m_timestampSubmitted[inFrameIndex])
            {
                return;
            }

            std::uint64_t timestamps[2] = {0, 0};
            vk::Result    result        = logicalDevice.getQueryPoolResults(
                m_timestampQueryPool,
                inFrameIndex * 2U,
                2U,
                sizeof(timestamps),
                timestamps,
                sizeof(std::uint64_t),
                vk::QueryResultFlagBits::e64
            );

            if (result != vk::Result::eSuccess)
            {
                return;
            }

            const double nanoseconds = static_cast<double>(timestamps[1] - timestamps[0]) * m_timestampPeriod;
            setGpuDelta(static_cast<float>(nanoseconds / 1'000'000.0));
        }

        void VulkanBackend::writeGpuTimestampStart(const vk::CommandBuffer& inCommandBuffer, std::uint32_t inFrameIndex)
        {
            if (!m_bTimestampsEnabled)
            {
                return;
            }

            const std::uint32_t query = inFrameIndex * 2U;
            inCommandBuffer.resetQueryPool(m_timestampQueryPool, query, 2U);
            inCommandBuffer.writeTimestamp(vk::PipelineStageFlagBits::eTopOfPipe, m_timestampQueryPool, query);
        }

        void VulkanBackend::writeGpuTimestampEnd(const vk::CommandBuffer& inCommandBuffer, std::uint32_t inFrameIndex)
        {
            if (!m_bTimestampsEnabled)
            {
                return;
            }

            inCommandBuffer
                .writeTimestamp(vk::PipelineStageFlagBits::eBottomOfPipe, m_timestampQueryPool, inFrameIndex * 2U + 1U);
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
            layoutBidings.counts.push_back(getResourceBudgetCount(Resource::Texture));
            layoutBidings.stages.push_back(vk::ShaderStageFlagBits::eFragment);

            VulkanDescriptorSetLayout::init(textureDescriptor.setLayout, logicalDevice, layoutBidings);

            const std::uint32_t setCount = std::max(1U, static_cast<std::uint32_t>(frames.size()));

            VulkanDescriptorPoolCreateInfo descriptorPoolCreateInfo;
            descriptorPoolCreateInfo.maxSets = setCount;
            descriptorPoolCreateInfo.sizes.push_back(
                {vk::DescriptorType::eCombinedImageSampler, getResourceBudgetCount(Resource::Texture) * setCount}
            );

            VulkanDescriptorPool::init(textureDescriptor.pool, logicalDevice, descriptorPoolCreateInfo);

            textureDescriptorSets.resize(setCount);
            for (vk::DescriptorSet& set : textureDescriptorSets)
            {
                VulkanDescriptorSetLayout::allocate(
                    set,
                    logicalDevice,
                    textureDescriptor.setLayout,
                    textureDescriptor.pool
                );
            }

            textureDescriptor.set = textureDescriptorSets.front();
        }

        void VulkanBackend::buildTextureData(const DrawTexture::List& inTextures)
        {
            if (inTextures.empty())
            {
                return;
            }

            for (VulkanFrame& frame : frames)
            {
                frame.wait();
            }

            VulkanTextureCreateInfo createInfo;
            createInfo.logicalDevice  = logicalDevice;
            createInfo.physicalDevice = physicalDevice;
            createInfo.commandBuffer  = mainCommandBuffer;
            createInfo.queue          = graphicsQueue;

            std::vector<vk::DescriptorImageInfo> infos;
            textures.clear();
            m_screenTextureId = Draw::InvalidId;

            for (const DrawTexture& texture : inTextures)
            {
                vk::DescriptorImageInfo info;
                info.imageLayout = vk::ImageLayout::eShaderReadOnlyOptimal;

                const bool bIsScreen = texture.reference.equals(SCREEN_TARGET_ID);

                if (bIsScreen && !swapchain.images.empty() && swapchain.images.front().targetImage.view)
                {
                    const VulkanImageInfo& target = swapchain.images.front().targetImage;
                    info.imageView                = target.view;
                    info.sampler                  = target.sampler;
                    textures.push_back(nullptr);
                    m_screenTextureId = texture.id;
                }
                else
                {
                    createInfo.image = texture.image;
                    textures.push_back(std::make_shared<VulkanTexture>(createInfo));
                    info.imageView = textures.back()->view;
                    info.sampler   = textures.back()->sampler;
                }

                infos.push_back(info);
            }

            for (vk::DescriptorSet set : textureDescriptorSets)
            {
                vk::WriteDescriptorSet write;
                write.dstSet          = set;
                write.dstBinding      = 0;
                write.dstArrayElement = 0;
                write.descriptorCount = static_cast<std::uint32_t>(infos.size());
                write.descriptorType  = vk::DescriptorType::eCombinedImageSampler;
                write.pImageInfo      = infos.data();

                logicalDevice.updateDescriptorSets(write, nullptr);
            }
        }

        void VulkanBackend::bindScreenTarget(const VulkanImageInfo& inTarget)
        {
            if (m_screenTextureId <= Draw::InvalidId || !inTarget.view || !inTarget.sampler)
            {
                return;
            }

            vk::DescriptorImageInfo info;
            info.imageLayout = vk::ImageLayout::eShaderReadOnlyOptimal;
            info.imageView   = inTarget.view;
            info.sampler     = inTarget.sampler;

            vk::WriteDescriptorSet set;
            set.dstSet          = getTextureDescriptorSet();
            set.dstBinding      = 0;
            set.dstArrayElement = static_cast<std::uint32_t>(m_screenTextureId);
            set.descriptorCount = 1;
            set.descriptorType  = vk::DescriptorType::eCombinedImageSampler;
            set.pImageInfo      = &info;

            logicalDevice.updateDescriptorSets(set, nullptr);
        }

        void VulkanBackend::destroyTextureData()
        {
            textures.clear();
            textureDescriptorSets.clear();

            logicalDevice.destroyDescriptorSetLayout(textureDescriptor.setLayout);
            logicalDevice.destroyDescriptorPool(textureDescriptor.pool);

            textureDescriptor.set       = nullptr;
            textureDescriptor.setLayout = nullptr;
            textureDescriptor.pool      = nullptr;
        }
    }
}