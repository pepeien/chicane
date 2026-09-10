#include "Chicane/Renderer/Backend/Vulkan/Layer/Scene.hpp"

#include "Chicane/Renderer/Backend/Vulkan.hpp"
#include "Chicane/Renderer/Backend/Vulkan/CommandBuffer/Worker.hpp"
#include "Chicane/Renderer/Backend/Vulkan/Descriptor/Pool.hpp"
#include "Chicane/Renderer/Backend/Vulkan/Descriptor/SetLayout.hpp"
#include "Chicane/Renderer/Backend/Vulkan/Descriptor/SetLayout/BidingsCreateInfo.hpp"
#include "Chicane/Renderer/Backend/Vulkan/Image.hpp"
#include "Chicane/Renderer/Backend/Vulkan/Image/Sampler/CreateInfo.hpp"
#include "Chicane/Renderer/Backend/Vulkan/Layer/Scene/Foreground.hpp"
#include "Chicane/Renderer/Backend/Vulkan/Layer/Scene/Mesh.hpp"
#include "Chicane/Renderer/Backend/Vulkan/Layer/Scene/Line.hpp"
#include "Chicane/Renderer/Backend/Vulkan/Layer/Scene/Shadow.hpp"
#include "Chicane/Renderer/Backend/Vulkan/Layer/Scene/Sky.hpp"

namespace Chicane
{
    namespace Renderer
    {
        VulkanLScene::VulkanLScene()
            : Layer(SCENE_LAYER_ID)
        {}

        void VulkanLScene::onInit()
        {
            buildModelVertexBuffer();
            buildModelIndexBuffer();
            buildShadowImage();
            buildSkyImage();
            buildLayers();
        }

        void VulkanLScene::onDestruction()
        {
            destroySkyImage();
            destroyShadowImage();
            destroyModelData();
        }

        void VulkanLScene::onLoad(DrawPolyType inType, const DrawPolyResource& inResource)
        {
            if (inType == DrawPolyType::e3D)
            {
                buildModelVertexData(inResource.getVertices());
                buildModelIndexData(inResource.getIndices());
            }
        }

        void VulkanLScene::buildModelVertexBuffer()
        {
            VulkanBackend* backend = getBackend<VulkanBackend>();

            VulkanBufferCreateInfo createInfo;
            createInfo.physicalDevice = backend->physicalDevice;
            createInfo.logicalDevice  = backend->logicalDevice;
            createInfo.size           = m_backend->getResourceBudget(Resource::SceneVertices);
            createInfo.usage          = vk::BufferUsageFlagBits::eTransferDst | vk::BufferUsageFlagBits::eVertexBuffer;
            createInfo.memoryProperties = vk::MemoryPropertyFlagBits::eDeviceLocal;

            modelVertexBuffer.init(createInfo);
        }

        void VulkanLScene::buildModelVertexData(const Vertex::List& inVertices)
        {
            VulkanBackend* backend = getBackend<VulkanBackend>();

            VulkanBufferCreateInfo createInfo;
            createInfo.physicalDevice = backend->physicalDevice;
            createInfo.logicalDevice  = backend->logicalDevice;
            createInfo.size           = sizeof(Vertex) * inVertices.size();
            createInfo.usage          = vk::BufferUsageFlagBits::eTransferSrc;
            createInfo.memoryProperties =
                vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent;

            VulkanBuffer stagingBuffer;
            stagingBuffer.init(createInfo);

            void* writeLocation = backend->logicalDevice.mapMemory(stagingBuffer.memory, 0, createInfo.size);
            memcpy(writeLocation, inVertices.data(), createInfo.size);
            backend->logicalDevice.unmapMemory(stagingBuffer.memory);

            stagingBuffer.copy(modelVertexBuffer, createInfo.size, backend->graphicsQueue, backend->mainCommandBuffer);
            stagingBuffer.destroy(backend->logicalDevice);
        }

        void VulkanLScene::buildModelIndexBuffer()
        {
            VulkanBackend* backend = getBackend<VulkanBackend>();

            VulkanBufferCreateInfo createInfo;
            createInfo.physicalDevice   = backend->physicalDevice;
            createInfo.logicalDevice    = backend->logicalDevice;
            createInfo.size             = m_backend->getResourceBudget(Resource::SceneIndices);
            createInfo.usage            = vk::BufferUsageFlagBits::eTransferDst | vk::BufferUsageFlagBits::eIndexBuffer;
            createInfo.memoryProperties = vk::MemoryPropertyFlagBits::eDeviceLocal;

            modelIndexBuffer.init(createInfo);
        }

        void VulkanLScene::buildModelIndexData(const Vertex::Indices& inIndices)
        {
            VulkanBackend* backend = getBackend<VulkanBackend>();

            VulkanBufferCreateInfo createInfo;
            createInfo.physicalDevice = backend->physicalDevice;
            createInfo.logicalDevice  = backend->logicalDevice;
            createInfo.size           = sizeof(Vertex::Index) * inIndices.size();
            createInfo.usage          = vk::BufferUsageFlagBits::eTransferSrc;
            createInfo.memoryProperties =
                vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent;

            VulkanBuffer stagingBuffer;
            stagingBuffer.init(createInfo);

            void* writeLocation = backend->logicalDevice.mapMemory(stagingBuffer.memory, 0, createInfo.size);
            memcpy(writeLocation, inIndices.data(), createInfo.size);
            backend->logicalDevice.unmapMemory(stagingBuffer.memory);

            stagingBuffer.copy(modelIndexBuffer, createInfo.size, backend->graphicsQueue, backend->mainCommandBuffer);
            stagingBuffer.destroy(backend->logicalDevice);
        }

        void VulkanLScene::destroyModelData()
        {
            VulkanBackend* backend = getBackend<VulkanBackend>();

            modelVertexBuffer.destroy(backend->logicalDevice);
            modelIndexBuffer.destroy(backend->logicalDevice);
        }

        void VulkanLScene::buildShadowImage()
        {
            VulkanBackend* backend = getBackend<VulkanBackend>();

            shadowImage.format = VulkanImage::findSupportedFormat(
                backend->physicalDevice,
                {vk::Format::eD32Sfloat, vk::Format::eD24UnormS8Uint},
                vk::ImageTiling::eOptimal,
                vk::FormatFeatureFlagBits::eDepthStencilAttachment
            );
            shadowImage.extent = vk::Extent2D{SHADOW_MAP_WIDTH, SHADOW_MAP_HEIGHT};

            VulkanImageCreateInfo instanceCreateInfo;
            instanceCreateInfo.width  = SHADOW_MAP_WIDTH;
            instanceCreateInfo.height = SHADOW_MAP_HEIGHT;
            instanceCreateInfo.count  = SHADOW_CASCADE_COUNT;
            instanceCreateInfo.tiling = vk::ImageTiling::eOptimal;
            instanceCreateInfo.flags  = vk::ImageCreateFlagBits();
            instanceCreateInfo.usage =
                vk::ImageUsageFlagBits::eDepthStencilAttachment | vk::ImageUsageFlagBits::eSampled;
            instanceCreateInfo.format        = shadowImage.format;
            instanceCreateInfo.logicalDevice = backend->logicalDevice;
            VulkanImage::initInstance(shadowImage.instance, instanceCreateInfo);

            vk::SamplerCreateInfo samplerCreateInfo;
            samplerCreateInfo.flags                   = vk::SamplerCreateFlags();
            samplerCreateInfo.minFilter               = vk::Filter::eNearest;
            samplerCreateInfo.magFilter               = vk::Filter::eNearest;
            samplerCreateInfo.mipmapMode              = vk::SamplerMipmapMode::eNearest;
            samplerCreateInfo.mipLodBias              = 0.0f;
            samplerCreateInfo.addressModeU            = vk::SamplerAddressMode::eClampToBorder;
            samplerCreateInfo.addressModeV            = vk::SamplerAddressMode::eClampToBorder;
            samplerCreateInfo.addressModeW            = vk::SamplerAddressMode::eClampToBorder;
            samplerCreateInfo.anisotropyEnable        = false;
            samplerCreateInfo.maxAnisotropy           = 1.0f;
            samplerCreateInfo.borderColor             = vk::BorderColor::eFloatOpaqueWhite;
            samplerCreateInfo.unnormalizedCoordinates = false;
            samplerCreateInfo.compareEnable           = false;
            samplerCreateInfo.compareOp               = vk::CompareOp::eLess;
            samplerCreateInfo.minLod                  = 0.0f;
            samplerCreateInfo.maxLod                  = 1.0f;
            shadowImage.sampler                       = backend->logicalDevice.createSampler(samplerCreateInfo);

            VulkanImageMemoryCreateInfo memoryCreateInfo;
            memoryCreateInfo.properties     = vk::MemoryPropertyFlagBits::eDeviceLocal;
            memoryCreateInfo.logicalDevice  = backend->logicalDevice;
            memoryCreateInfo.physicalDevice = backend->physicalDevice;
            VulkanImage::initMemory(shadowImage.memory, shadowImage.instance, memoryCreateInfo);

            VulkanImageViewCreateInfo arrayViewCreateInfo;
            arrayViewCreateInfo.count         = SHADOW_CASCADE_COUNT;
            arrayViewCreateInfo.type          = vk::ImageViewType::e2DArray;
            arrayViewCreateInfo.aspect        = vk::ImageAspectFlagBits::eDepth;
            arrayViewCreateInfo.format        = instanceCreateInfo.format;
            arrayViewCreateInfo.logicalDevice = backend->logicalDevice;
            VulkanImage::initView(shadowImage.view, shadowImage.instance, arrayViewCreateInfo);

            for (std::uint32_t cascade = 0; cascade < SHADOW_CASCADE_COUNT; ++cascade)
            {
                VulkanImageViewCreateInfo layerViewCreateInfo;
                layerViewCreateInfo.count          = 1;
                layerViewCreateInfo.baseArrayLayer = cascade;
                layerViewCreateInfo.type           = vk::ImageViewType::e2D;
                layerViewCreateInfo.aspect         = vk::ImageAspectFlagBits::eDepth;
                layerViewCreateInfo.format         = instanceCreateInfo.format;
                layerViewCreateInfo.logicalDevice  = backend->logicalDevice;
                VulkanImage::initView(shadowLayerViews[cascade], shadowImage.instance, layerViewCreateInfo);
            }

            VulkanImage::transitionLayout(
                backend->mainCommandBuffer,
                backend->graphicsQueue,
                shadowImage.instance,
                vk::ImageLayout::eUndefined,
                vk::ImageLayout::eDepthStencilReadOnlyOptimal,
                SHADOW_CASCADE_COUNT,
                1,
                vk::ImageAspectFlagBits::eDepth
            );

            shadowImageInfo.imageLayout = vk::ImageLayout::eDepthStencilReadOnlyOptimal;
            shadowImageInfo.imageView   = shadowImage.view;
            shadowImageInfo.sampler     = shadowImage.sampler;
        }

        void VulkanLScene::destroyShadowImage()
        {
            VulkanBackend* backend = getBackend<VulkanBackend>();

            for (std::uint32_t cascade = 0; cascade < SHADOW_CASCADE_COUNT; ++cascade)
            {
                backend->logicalDevice.destroyImageView(shadowLayerViews[cascade]);
                shadowLayerViews[cascade] = nullptr;
            }

            backend->logicalDevice.freeMemory(shadowImage.memory);
            backend->logicalDevice.destroyImage(shadowImage.instance);
            backend->logicalDevice.destroyImageView(shadowImage.view);
            backend->logicalDevice.destroySampler(shadowImage.sampler);
        }

        void VulkanLScene::buildSkyImage()
        {
            VulkanBackend* backend = getBackend<VulkanBackend>();

            constexpr std::uint32_t kFaceCount = 6;

            skyImage.format = vk::Format::eR8G8B8A8Unorm;
            skyImage.extent = vk::Extent2D{1, 1};

            VulkanImageCreateInfo instanceCreateInfo;
            instanceCreateInfo.width         = 1;
            instanceCreateInfo.height        = 1;
            instanceCreateInfo.count         = kFaceCount;
            instanceCreateInfo.tiling        = vk::ImageTiling::eOptimal;
            instanceCreateInfo.flags         = vk::ImageCreateFlagBits::eCubeCompatible;
            instanceCreateInfo.usage         = vk::ImageUsageFlagBits::eTransferDst | vk::ImageUsageFlagBits::eSampled;
            instanceCreateInfo.format        = skyImage.format;
            instanceCreateInfo.logicalDevice = backend->logicalDevice;
            VulkanImage::initInstance(skyImage.instance, instanceCreateInfo);

            VulkanImageSamplerCreateInfo samplerCreateInfo;
            samplerCreateInfo.addressMode   = vk::SamplerAddressMode::eClampToEdge;
            samplerCreateInfo.borderColor   = vk::BorderColor::eIntTransparentBlack;
            samplerCreateInfo.logicalDevice = backend->logicalDevice;
            VulkanImage::initSampler(skyImage.sampler, samplerCreateInfo);

            VulkanImageMemoryCreateInfo memoryCreateInfo;
            memoryCreateInfo.properties     = vk::MemoryPropertyFlagBits::eDeviceLocal;
            memoryCreateInfo.logicalDevice  = backend->logicalDevice;
            memoryCreateInfo.physicalDevice = backend->physicalDevice;
            VulkanImage::initMemory(skyImage.memory, skyImage.instance, memoryCreateInfo);

            VulkanImageViewCreateInfo viewCreateInfo;
            viewCreateInfo.count         = kFaceCount;
            viewCreateInfo.type          = vk::ImageViewType::eCube;
            viewCreateInfo.aspect        = vk::ImageAspectFlagBits::eColor;
            viewCreateInfo.format        = skyImage.format;
            viewCreateInfo.logicalDevice = backend->logicalDevice;
            VulkanImage::initView(skyImage.view, skyImage.instance, viewCreateInfo);

            VulkanImage::transitionLayout(
                backend->mainCommandBuffer,
                backend->graphicsQueue,
                skyImage.instance,
                vk::ImageLayout::eUndefined,
                vk::ImageLayout::eTransferDstOptimal,
                kFaceCount,
                1
            );

            VulkanCommandBufferWorker::startJob(backend->mainCommandBuffer);
            vk::ClearColorValue       clearColor(0.15f, 0.15f, 0.18f, 1.0f);
            vk::ImageSubresourceRange range;
            range.aspectMask     = vk::ImageAspectFlagBits::eColor;
            range.baseMipLevel   = 0;
            range.levelCount     = 1;
            range.baseArrayLayer = 0;
            range.layerCount     = kFaceCount;
            backend->mainCommandBuffer
                .clearColorImage(skyImage.instance, vk::ImageLayout::eTransferDstOptimal, clearColor, range);
            VulkanCommandBufferWorker::endJob(backend->mainCommandBuffer, backend->graphicsQueue, "Clear Default Sky");

            VulkanImage::transitionLayout(
                backend->mainCommandBuffer,
                backend->graphicsQueue,
                skyImage.instance,
                vk::ImageLayout::eTransferDstOptimal,
                vk::ImageLayout::eShaderReadOnlyOptimal,
                kFaceCount,
                1
            );

            skyImageInfo.imageLayout = vk::ImageLayout::eShaderReadOnlyOptimal;
            skyImageInfo.imageView   = skyImage.view;
            skyImageInfo.sampler     = skyImage.sampler;
        }

        void VulkanLScene::destroySkyImage()
        {
            VulkanBackend* backend = getBackend<VulkanBackend>();

            backend->logicalDevice.freeMemory(skyImage.memory);
            backend->logicalDevice.destroyImage(skyImage.instance);
            backend->logicalDevice.destroyImageView(skyImage.view);
            backend->logicalDevice.destroySampler(skyImage.sampler);
        }

        void VulkanLScene::setSkyImageInfo(const vk::DescriptorImageInfo& inInfo)
        {
            skyImageInfo = inInfo;

            if (VulkanLSceneMesh* mesh = m_backend->getLayer<VulkanLSceneMesh>(SCENE_MESH_LAYER_ID))
            {
                mesh->updateSkyDescriptors(skyImageInfo);
            }

            if (VulkanLSceneForeground* foreground =
                    m_backend->getLayer<VulkanLSceneForeground>(SCENE_FOREGROUND_LAYER_ID))
            {
                foreground->updateSkyDescriptors(skyImageInfo);
            }
        }

        void VulkanLScene::buildLayers()
        {
            ListPush<Layer*> settings;
            settings.strategy = ListPushStrategy::Back;

            m_backend->addLayer<VulkanLSceneSky>(settings);
            m_backend->addLayer<VulkanLSceneShadow>(settings);
            m_backend->addLayer<VulkanLSceneMesh>(settings);
            m_backend->addLayer<VulkanLSceneLine>(settings);
            m_backend->addLayer<VulkanLSceneForeground>(settings);
        }
    }
}