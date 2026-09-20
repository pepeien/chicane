#include "Backend/Vulkan.hpp"

#include <algorithm>
#include <array>
#include <utility>

#include "Chicane/Renderer/Instance.hpp"
#include "Backend/Vulkan/CommandBuffer.hpp"
#include "Backend/Vulkan/CommandBuffer/Pool.hpp"
#include "Backend/Vulkan/Debug.hpp"
#include "Backend/Vulkan/Descriptor/Pool.hpp"
#include "Backend/Vulkan/Descriptor/SetLayout.hpp"
#include "Backend/Vulkan/Descriptor/SetLayout/BidingsCreateInfo.hpp"
#include "Backend/Vulkan/Device.hpp"
#include "Backend/Vulkan/Queue.hpp"
#include "Backend/Vulkan/Instance.hpp"
#include "Chicane/Renderer/Shader/Bindings.hpp"
#include "Chicane/Renderer/Layer/Scene.hpp"
#include "Chicane/Renderer/Layer/UI.hpp"
#include "Backend/Vulkan/RHI/Device.hpp"
#include "Backend/Vulkan/Surface.hpp"
#include "Backend/Vulkan/Swapchain.hpp"

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
              m_bIsTimestampsEnabled(false),
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
            allocator.init(instance, physicalDevice, logicalDevice);
            updateResourceBudget();
            buildQueues();
            buildCommandPool();
            buildMainCommandBuffer();
            buildSwapchain();
            buildFrames();
            buildTimestampQueries();
            buildTextureDescriptor();
            m_rhi = std::make_unique<VulkanRHIDevice>(this);
            RHI::SamplerCreateInfo sampler;
            sampler.minFilter = RHI::SamplerFilter::Linear;
            sampler.magFilter = RHI::SamplerFilter::Linear;
            sampler.address   = RHI::SamplerAddress::ClampToEdge;
            m_linearSampler   = m_rhi->createSampler(sampler);
            bloom.init(m_rhi.get());
            bloom.resize(swapchain.extent.width, swapchain.extent.height, m_rhi->sceneColorFormat());
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

            destroyLayers();
            bloom.destroy();
            if (m_linearSampler.handle)
            {
                m_rhi->destroySampler(m_linearSampler);
                m_linearSampler = {};
            }
            releaseRhiWraps();
            m_rhi.reset();

            destroyCommandPool();
            destroyTimestampQueries();
            destroySwapchain();
            destroyFrames();
            destroyTextureData();

            allocator.destroy();
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
            if (!swapchain.instance || getRenderer()->getWindow()->isMinimized())
            {
                return;
            }

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

            nextFrame.commandBuffer.reset();
            bindScreenTarget(nextImage.targetImage);

            nextFrame.begin(inFrame, nextImage);
            {
                writeGpuTimestampStart(nextFrame.commandBuffer, m_currentFrameIndex);

                fillRhiFrame(nextFrame, nextImage);
                renderLayers(
                    inFrame,
                    &m_rhiFrame,
                    [](const Layer* inLayer) { return !inLayer->getId().equals(UI_LAYER_ID); }
                );
                if (!isScreenComposited(inFrame))
                {
                    bloom.apply(
                        m_rhiFrame.commands,
                        m_rhiFrame.sceneColor,
                        m_rhiFrame.presentColor,
                        m_rhiFrame.width,
                        m_rhiFrame.height,
                        inFrame.hasFeature(RendererFeature::HDR),
                        m_rhiFrame.frameIndex
                    );
                }
                renderLayers(
                    inFrame,
                    &m_rhiFrame,
                    [](const Layer* inLayer) { return inLayer->getId().equals(UI_LAYER_ID); }
                );

                writeGpuTimestampEnd(nextFrame.commandBuffer, m_currentFrameIndex);
                m_rhiFrame.commands->preparePresent(m_rhiFrame.presentColor);
            }
            nextFrame.end();

            const std::array<vk::PipelineStageFlags, 1> waitStages = {
                vk::PipelineStageFlagBits::eColorAttachmentOutput
            };

            vk::SubmitInfo submitInfo;
            submitInfo.waitSemaphoreCount   = 1;
            submitInfo.pWaitSemaphores      = &nextFrame.imageAvailableSemaphore;
            submitInfo.pWaitDstStageMask    = waitStages.data();
            submitInfo.commandBufferCount   = 1;
            submitInfo.pCommandBuffers      = &nextFrame.commandBuffer;
            submitInfo.signalSemaphoreCount = 1;
            submitInfo.pSignalSemaphores    = &nextImage.renderFineshedSemaphore;

            vk::Result submitResult = graphicsQueue.submit(1, &submitInfo, nextFrame.fence);
            if (submitResult != vk::Result::eSuccess)
            {
                throw std::runtime_error("Queue submit failed");
            }

            if (m_bIsTimestampsEnabled)
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

        void VulkanBackend::releaseBoundDescriptors()
        {
            for (VulkanFrame& frame : frames)
            {
                frame.wait();
                frame.commandBuffer.reset();
            }
        }

        vk::Format VulkanBackend::getSceneColorFormat() const
        {
            return vk::Format::eR16G16B16A16Sfloat;
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

        RHI::Viewport VulkanBackend::getRHIViewport(Layer* inLayer) const
        {
            const vk::Viewport viewport = getVkViewport(inLayer);

            RHI::Viewport result;
            result.size     = Vec2(viewport.width, viewport.height);
            result.position = Vec2(viewport.x, viewport.y);
            result.depth    = Vec2(viewport.minDepth, viewport.maxDepth);

            return result;
        }

        RHI::Scissor VulkanBackend::getRHIScissor(Layer* inLayer) const
        {
            const vk::Viewport viewport = getVkViewport(inLayer);

            RHI::Scissor result;
            result.x      = static_cast<std::int32_t>(viewport.x);
            result.y      = static_cast<std::int32_t>(viewport.y);
            result.width  = static_cast<std::uint32_t>(viewport.width);
            result.height = static_cast<std::uint32_t>(viewport.height);

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
            setVRAM(VulkanAllocator::queryDedicatedHeapSize(physicalDevice));
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

        vk::Extent2D VulkanBackend::getSwapchainFallbackExtent() const
        {
            const Window* window = getRenderer()->getWindow();
            if (!window)
            {
                return {};
            }

            const Vec<2, std::uint32_t>& size = window->getSize();

            return {size.x, size.y};
        }

        void VulkanBackend::setupSwapchainImages()
        {
            for (VulkanSwapchainImage& image : swapchain.images)
            {
                // Sync
                image.setupSync();

                image.allocator = &allocator;

                // Images
                image.setupColorImage(swapchain.colorFormat, swapchain.extent);
                image.setupTargetImage(getSceneColorFormat(), swapchain.extent);
                image.setupDepthImage(swapchain.depthFormat, swapchain.extent);
            }
        }

        void VulkanBackend::buildSwapchain()
        {
            if (!VulkanSwapchain::init(
                    swapchain,
                    physicalDevice,
                    logicalDevice,
                    surface,
                    {},
                    getSwapchainFallbackExtent()
                ))
            {
                throw std::runtime_error("Failed to create the swapchain");
            }

            setupSwapchainImages();
        }

        void VulkanBackend::destroySwapchain()
        {
            logicalDevice.waitIdle();

            for (VulkanSwapchainImage& image : swapchain.images)
            {
                image.destroy();
            }

            swapchain.images.clear();

            if (swapchain.instance)
            {
                logicalDevice.destroySwapchainKHR(swapchain.instance);
                swapchain.instance = nullptr;
            }

            shutdownLayers();
        }

        void VulkanBackend::rebuildSwapchain()
        {
            if (!swapchain.instance || getRenderer()->getWindow()->isMinimized())
            {
                return;
            }

            VulkanSwapchainSupportDetails supportDetails;
            VulkanSwapchain::querySupport(supportDetails, physicalDevice, surface);

            const vk::Extent2D extent =
                VulkanSwapchain::chooseExtent(supportDetails.capabilities, getSwapchainFallbackExtent());
            if (extent.width == 0 || extent.height == 0)
            {
                return;
            }

            logicalDevice.waitIdle();

            VulkanSwapchainBundle next = {};
            if (!VulkanSwapchain::init(
                    next,
                    physicalDevice,
                    logicalDevice,
                    surface,
                    swapchain.instance,
                    getSwapchainFallbackExtent()
                ))
            {
                return;
            }

            bloom.destroyImages();

            for (VulkanSwapchainImage& image : swapchain.images)
            {
                image.destroy();
            }

            swapchain.images.clear();
            shutdownLayers();

            if (swapchain.instance)
            {
                logicalDevice.destroySwapchainKHR(swapchain.instance);
            }

            swapchain = std::move(next);
            setupSwapchainImages();
            rebuildLayers();
            bloom.resize(swapchain.extent.width, swapchain.extent.height, m_rhi->sceneColorFormat());
        }

        void VulkanBackend::buildFrames()
        {
            frames.resize(m_renderer->getFrameInFlighCount());

            for (VulkanFrame& frame : frames)
            {
                frame.logicalDevice  = logicalDevice;
                frame.physicalDevice = physicalDevice;
                frame.allocator      = &allocator;

                // Commandbuffer
                frame.setupCommandBuffer(m_mainCommandPool);

                // Sync
                frame.setupSync();

                // Data
                frame.setupCameraData();
                frame.setupLightData();
                frame.setup2DData(getResourceBudget(Resource::UIInstances));
                frame.setup3DData(getResourceBudget(Resource::SceneInstances));
                frame.setupParticleData(sizeof(DrawParticle) * MAX_PARTICLES);
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
            m_bIsTimestampsEnabled = false;
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

            m_timestampQueryPool   = logicalDevice.createQueryPool(createInfo);
            m_bIsTimestampsEnabled = true;
        }

        void VulkanBackend::destroyTimestampQueries()
        {
            if (m_timestampQueryPool)
            {
                logicalDevice.destroyQueryPool(m_timestampQueryPool);
                m_timestampQueryPool = nullptr;
            }

            m_bIsTimestampsEnabled = false;
            m_timestampSubmitted.clear();
        }

        void VulkanBackend::resolveGpuTimestamp(std::uint32_t inFrameIndex)
        {
            if (!m_bIsTimestampsEnabled || !m_timestampSubmitted[inFrameIndex])
            {
                return;
            }

            std::array<std::uint64_t, 2> timestamps = {0, 0};
            vk::Result                   result     = logicalDevice.getQueryPoolResults(
                m_timestampQueryPool,
                inFrameIndex * 2U,
                2U,
                sizeof(timestamps),
                timestamps.data(),
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
            if (!m_bIsTimestampsEnabled)
            {
                return;
            }

            const std::uint32_t query = inFrameIndex * 2U;
            inCommandBuffer.resetQueryPool(m_timestampQueryPool, query, 2U);
            inCommandBuffer.writeTimestamp(vk::PipelineStageFlagBits::eTopOfPipe, m_timestampQueryPool, query);
        }

        void VulkanBackend::writeGpuTimestampEnd(const vk::CommandBuffer& inCommandBuffer, std::uint32_t inFrameIndex)
        {
            if (!m_bIsTimestampsEnabled)
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
            addLayer<LScene>(settings);

            settings.strategy = ListPushStrategy::Back;
            addLayer<LUI>(settings);
        }

        void VulkanBackend::buildTextureDescriptor()
        {
            VulkanDescriptorSetLayoutBidingsCreateInfo layoutBidings;
            layoutBidings.count = 1;

            layoutBidings.indices.push_back(RHI_BINDING_TEXTURES);
            layoutBidings.types.push_back(vk::DescriptorType::eCombinedImageSampler);
            layoutBidings.counts.push_back(getResourceBudgetCount(Resource::Texture));
            layoutBidings.stages.push_back(vk::ShaderStageFlagBits::eFragment);
            layoutBidings.bindingFlags.push_back(vk::DescriptorBindingFlagBits::ePartiallyBound);

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

        bool VulkanBackend::needsTextureUpload(const DrawTexture& inTexture) const
        {
            if (inTexture.id <= Draw::InvalidId)
            {
                return false;
            }

            if (inTexture.reference.equals(SCREEN_TARGET_ID))
            {
                return m_screenTextureId != inTexture.id;
            }

            const std::size_t index = static_cast<std::size_t>(inTexture.id);
            if (index >= textures.size())
            {
                return true;
            }

            const std::shared_ptr<VulkanTexture>& slot = textures[index];

            return !slot || !slot->matches(inTexture);
        }

        void VulkanBackend::buildTextureData(const DrawTexture::List& inTextures)
        {
            if (inTextures.empty())
            {
                return;
            }

            Draw::Id maxId = Draw::InvalidId;
            for (const DrawTexture& texture : inTextures)
            {
                maxId = std::max(maxId, texture.id);
            }

            if (maxId > Draw::InvalidId)
            {
                const std::size_t needed = static_cast<std::size_t>(maxId) + 1;
                if (textures.size() < needed)
                {
                    textures.resize(needed);
                }
            }

            const std::uint32_t slotCount    = getResourceBudgetCount(Resource::Texture);
            bool                bNeedsUpload = false;
            for (const DrawTexture& texture : inTextures)
            {
                if (texture.id <= Draw::InvalidId || static_cast<std::uint32_t>(texture.id) >= slotCount)
                {
                    continue;
                }

                if (needsTextureUpload(texture))
                {
                    bNeedsUpload = true;

                    break;
                }
            }

            if (!bNeedsUpload)
            {
                return;
            }

            releaseBoundDescriptors();

            VulkanTextureCreateInfo createInfo;
            createInfo.logicalDevice  = logicalDevice;
            createInfo.physicalDevice = physicalDevice;
            createInfo.commandBuffer  = mainCommandBuffer;
            createInfo.queue          = graphicsQueue;
            createInfo.allocator      = &allocator;

            for (const DrawTexture& texture : inTextures)
            {
                if (texture.id <= Draw::InvalidId || static_cast<std::uint32_t>(texture.id) >= slotCount)
                {
                    continue;
                }

                if (!needsTextureUpload(texture))
                {
                    continue;
                }

                vk::DescriptorImageInfo info;
                info.imageLayout = vk::ImageLayout::eShaderReadOnlyOptimal;

                const bool bIsScreen = texture.reference.equals(SCREEN_TARGET_ID);
                if (bIsScreen)
                {
                    m_screenTextureId = texture.id;
                    if (!swapchain.images.empty() && swapchain.images.front().targetImage.view)
                    {
                        const VulkanImageInfo& target                  = swapchain.images.front().targetImage;
                        info.imageView                                 = target.view;
                        info.sampler                                   = target.sampler;
                        textures[static_cast<std::size_t>(texture.id)] = nullptr;
                        writeTextureDescriptor(texture.id, info);
                    }

                    continue;
                }

                std::shared_ptr<VulkanTexture>& slot = textures[static_cast<std::size_t>(texture.id)];
                createInfo.texture                   = &texture;
                createInfo.image                     = texture.image;
                createInfo.residentMinMip            = texture.residentMinMip;
                slot                                 = std::make_shared<VulkanTexture>(createInfo);

                info.imageView = slot->view;
                info.sampler   = slot->sampler;
                writeTextureDescriptor(texture.id, info);
            }
        }

        void VulkanBackend::writeTextureDescriptor(Draw::Id inId, const vk::DescriptorImageInfo& inInfo)
        {
            if (inId <= Draw::InvalidId)
            {
                return;
            }

            for (vk::DescriptorSet set : textureDescriptorSets)
            {
                vk::WriteDescriptorSet write;
                write.dstSet          = set;
                write.dstBinding      = RHI_BINDING_TEXTURES;
                write.dstArrayElement = static_cast<std::uint32_t>(inId);
                write.descriptorCount = 1;
                write.descriptorType  = vk::DescriptorType::eCombinedImageSampler;
                write.pImageInfo      = &inInfo;

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
            set.dstBinding      = RHI_BINDING_TEXTURES;
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

        void VulkanBackend::releaseRhiWraps()
        {
            if (!m_rhi)
            {
                return;
            }

            auto killBuffer = [&](RHI::Buffer& inBuffer)
            {
                if (inBuffer.handle)
                {
                    m_rhi->destroyBuffer(inBuffer);
                    inBuffer = {};
                }
            };
            auto killImage = [&](RHI::Image& inImage)
            {
                if (inImage.handle)
                {
                    m_rhi->destroyImage(inImage);
                    inImage = {};
                }
            };
            auto killGroup = [&](RHI::BindGroup& inGroup)
            {
                if (inGroup.handle)
                {
                    m_rhi->destroyBindGroup(inGroup);
                    inGroup = {};
                }
            };

            killBuffer(m_rhiFrame.cameraBuffer);
            killBuffer(m_rhiFrame.lightBuffer);
            killBuffer(m_rhiFrame.instance3DBuffer);
            killBuffer(m_rhiFrame.instance2DBuffer);
            killBuffer(m_rhiFrame.particleBuffer);
            killImage(m_rhiFrame.sceneColor);
            killImage(m_rhiFrame.sceneDepth);
            killImage(m_rhiFrame.presentColor);
            killGroup(m_rhiFrame.textureTable);
        }

        void VulkanBackend::fillRhiFrame(VulkanFrame& inFrame, const VulkanSwapchainImage& inImage)
        {
            auto* device = static_cast<VulkanRHIDevice*>(m_rhi.get());
            releaseRhiWraps();

            device->setCommandBuffer(inFrame.commandBuffer);
            m_rhiFrame.commands      = device->commandList();
            m_rhiFrame.frameIndex    = m_currentFrameIndex;
            m_rhiFrame.width         = swapchain.extent.width;
            m_rhiFrame.height        = swapchain.extent.height;
            m_rhiFrame.linearSampler = m_linearSampler;

            VulkanBuffer camera;
            camera.instance         = inFrame.cameraResource.bufferInfo.buffer;
            m_rhiFrame.cameraBuffer = device->wrapBuffer(camera, inFrame.cameraResource.bufferInfo.range, nullptr);

            VulkanBuffer light;
            light.instance         = inFrame.lightResource.bufferInfo.buffer;
            m_rhiFrame.lightBuffer = device->wrapBuffer(light, inFrame.lightResource.bufferInfo.range, nullptr);

            VulkanBuffer instances;
            instances.instance = inFrame.poly3DResource.bufferInfo.buffer;
            m_rhiFrame.instance3DBuffer =
                device->wrapBuffer(instances, inFrame.poly3DResource.bufferInfo.range, nullptr);

            VulkanBuffer ui;
            ui.instance                 = inFrame.poly2DResource.bufferInfo.buffer;
            m_rhiFrame.instance2DBuffer = device->wrapBuffer(ui, inFrame.poly2DResource.bufferInfo.range, nullptr);

            VulkanBuffer particles;
            particles.instance = inFrame.particleResource.bufferInfo.buffer;
            m_rhiFrame.particleBuffer =
                device->wrapBuffer(particles, inFrame.particleResource.bufferInfo.range, nullptr);

            m_rhiFrame.sceneColor =
                device->wrapImage(inImage.targetImage, RHI::ImageKind::Color2D, RHI::ImageFormat::RGBA16F);
            m_rhiFrame.sceneDepth =
                device->wrapImage(inImage.depthImage, RHI::ImageKind::Depth2D, device->sceneDepthFormat());
            m_rhiFrame.presentColor =
                device->wrapImage(inImage.colorImage, RHI::ImageKind::Color2D, device->presentColorFormat());
            m_rhiFrame.textureTable = device->wrapDescriptorSet(getTextureDescriptorSet());

            auto* sceneColor       = static_cast<VulkanRHIImageData*>(m_rhiFrame.sceneColor.handle);
            sceneColor->bIsSampled = true;
            sceneColor->bHasColor  = true;
            device->rememberLayout(sceneColor, vk::ImageLayout::eColorAttachmentOptimal);

            auto* sceneDepth      = static_cast<VulkanRHIImageData*>(m_rhiFrame.sceneDepth.handle);
            sceneDepth->bHasDepth = true;
            device->rememberLayout(sceneDepth, vk::ImageLayout::eDepthStencilAttachmentOptimal);

            auto* presentColor      = static_cast<VulkanRHIImageData*>(m_rhiFrame.presentColor.handle);
            presentColor->bHasColor = true;
            presentColor->bPresent  = true;
        }
    }
}