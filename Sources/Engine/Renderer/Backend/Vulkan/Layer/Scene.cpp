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
            : Layer(ID)
        {}

        void VulkanLScene::onInit()
        {
            buildModelVertexBuffer();
            buildModelIndexBuffer();
            buildLayers();
        }

        void VulkanLScene::onDestruction()
        {
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
            createInfo.size           = sizeof(Vertex) * Backend::SCENE_VERTEX_COUNT;
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
            createInfo.size             = sizeof(Vertex::Index) * Backend::SCENE_INDEX_COUNT;
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