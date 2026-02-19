#include "Chicane/Renderer/Backend/Vulkan/Layer/Scene.hpp"

#include "Chicane/Renderer/Backend/Vulkan.hpp"
#include "Chicane/Renderer/Backend/Vulkan/Descriptor/Pool.hpp"
#include "Chicane/Renderer/Backend/Vulkan/Descriptor/SetLayout.hpp"
#include "Chicane/Renderer/Backend/Vulkan/Descriptor/SetLayout/BidingsCreateInfo.hpp"
#include "Chicane/Renderer/Backend/Vulkan/Layer/Scene/Mesh.hpp"
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
            buildLayers();
        }

        void VulkanLScene::onDestruction()
        {
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

            shadowImage.format = backend->swapchain.depthFormat;
            shadowImage.extent = backend->swapchain.extent;

            VulkanImageCreateInfo instanceCreateInfo = {};
            instanceCreateInfo.width                 = 512;
            instanceCreateInfo.height                = 512;
            instanceCreateInfo.count                 = 1;
            instanceCreateInfo.tiling                = vk::ImageTiling::eOptimal;
            instanceCreateInfo.flags                 = vk::ImageCreateFlagBits();
            instanceCreateInfo.usage =
                vk::ImageUsageFlagBits::eDepthStencilAttachment | vk::ImageUsageFlagBits::eSampled;
            instanceCreateInfo.format        = shadowImage.format;
            instanceCreateInfo.logicalDevice = backend->logicalDevice;
            VulkanImage::initInstance(shadowImage.instance, instanceCreateInfo);

            vk::SamplerCreateInfo samplerCreateInfo   = {};
            samplerCreateInfo.flags                   = vk::SamplerCreateFlags();
            samplerCreateInfo.minFilter               = vk::Filter::eLinear;
            samplerCreateInfo.magFilter               = vk::Filter::eLinear;
            samplerCreateInfo.mipmapMode              = vk::SamplerMipmapMode::eLinear;
            samplerCreateInfo.mipLodBias              = 0.0f;
            samplerCreateInfo.addressModeU            = vk::SamplerAddressMode::eClampToEdge;
            samplerCreateInfo.addressModeV            = vk::SamplerAddressMode::eClampToEdge;
            samplerCreateInfo.addressModeW            = vk::SamplerAddressMode::eClampToEdge;
            samplerCreateInfo.anisotropyEnable        = false;
            samplerCreateInfo.maxAnisotropy           = 1.0f;
            samplerCreateInfo.borderColor             = vk::BorderColor::eFloatOpaqueWhite;
            samplerCreateInfo.unnormalizedCoordinates = false;
            samplerCreateInfo.compareEnable           = true;
            samplerCreateInfo.compareOp               = vk::CompareOp::eLess;
            samplerCreateInfo.minLod                  = 0.0f;
            samplerCreateInfo.maxLod                  = 1.0f;
            samplerCreateInfo.unnormalizedCoordinates = false;
            shadowImage.sampler                       = backend->logicalDevice.createSampler(samplerCreateInfo);

            VulkanImageMemoryCreateInfo memoryCreateInfo = {};
            memoryCreateInfo.properties                  = vk::MemoryPropertyFlagBits::eDeviceLocal;
            memoryCreateInfo.logicalDevice               = backend->logicalDevice;
            memoryCreateInfo.physicalDevice              = backend->physicalDevice;
            VulkanImage::initMemory(shadowImage.memory, shadowImage.instance, memoryCreateInfo);

            VulkanImageViewCreateInfo viewCreateInfo = {};
            viewCreateInfo.count                     = instanceCreateInfo.count;
            viewCreateInfo.type                      = vk::ImageViewType::e2D;
            viewCreateInfo.aspect                    = vk::ImageAspectFlagBits::eDepth;
            viewCreateInfo.format                    = instanceCreateInfo.format;
            viewCreateInfo.logicalDevice             = backend->logicalDevice;
            VulkanImage::initView(shadowImage.view, shadowImage.instance, viewCreateInfo);

            shadowImageInfo.imageLayout = vk::ImageLayout::eDepthAttachmentStencilReadOnlyOptimal;
            shadowImageInfo.imageView   = shadowImage.view;
            shadowImageInfo.sampler     = shadowImage.sampler;
        }

        void VulkanLScene::destroyShadowImage()
        {
            VulkanBackend* backend = getBackend<VulkanBackend>();

            backend->logicalDevice.freeMemory(shadowImage.memory);
            backend->logicalDevice.destroyImage(shadowImage.instance);
            backend->logicalDevice.destroyImageView(shadowImage.view);
            backend->logicalDevice.destroySampler(shadowImage.sampler);
        }

        void VulkanLScene::buildLayers()
        {
            ListPush<Layer*> settings;
            settings.strategy = ListPushStrategy::Back;

            m_backend->addLayer<VulkanLSceneSky>(settings);
            m_backend->addLayer<VulkanLSceneShadow>(settings);
            m_backend->addLayer<VulkanLSceneMesh>(settings);
        }
    }
}