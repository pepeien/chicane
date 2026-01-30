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
            : Layer("Engine_Scene")
        {}

        VulkanLScene::~VulkanLScene()
        {
            deleteChildren();
            destroyModelData();
        }

        bool VulkanLScene::onInit()
        {
            buildModelVertexBuffer();
            buildModelIndexBuffer();
            buildLayers();

            return true;
        }

        void VulkanLScene::onLoad(DrawPolyType inType, const DrawPolyResource& inResource)
        {
            if (inType == DrawPolyType::e3D)
            {
                buildModelVertexData(inResource.getVertices());
                buildModelIndexData(inResource.getIndices());
            }
        }

        bool VulkanLScene::onSetup(const Frame& inFrame)
        {
            if (inFrame.getInstances3D().empty())
            {
                return false;
            }

            if (inFrame.get3DDraws().empty())
            {
                return false;
            }

            return true;
        }

        void VulkanLScene::buildModelVertexBuffer()
        {
            VulkanBufferCreateInfo createInfo;
            createInfo.physicalDevice = getBackend<VulkanBackend>()->physicalDevice;
            createInfo.logicalDevice  = getBackend<VulkanBackend>()->logicalDevice;
            createInfo.size           = sizeof(Vertex) * 2000000;
            createInfo.usage          = vk::BufferUsageFlagBits::eTransferDst | vk::BufferUsageFlagBits::eVertexBuffer;
            createInfo.memoryProperties = vk::MemoryPropertyFlagBits::eDeviceLocal;

            modelVertexBuffer.init(createInfo);
        }

        void VulkanLScene::buildModelVertexData(const Vertex::List& inVertices)
        {
            VulkanBufferCreateInfo createInfo;
            createInfo.physicalDevice = getBackend<VulkanBackend>()->physicalDevice;
            createInfo.logicalDevice  = getBackend<VulkanBackend>()->logicalDevice;
            createInfo.size           = sizeof(Vertex) * inVertices.size();
            createInfo.usage          = vk::BufferUsageFlagBits::eTransferSrc;
            createInfo.memoryProperties =
                vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent;

            VulkanBuffer stagingBuffer;
            stagingBuffer.init(createInfo);

            void* writeLocation =
                getBackend<VulkanBackend>()->logicalDevice.mapMemory(stagingBuffer.memory, 0, createInfo.size);
            memcpy(writeLocation, inVertices.data(), createInfo.size);
            getBackend<VulkanBackend>()->logicalDevice.unmapMemory(stagingBuffer.memory);

            stagingBuffer.copy(
                modelVertexBuffer,
                createInfo.size,
                getBackend<VulkanBackend>()->graphicsQueue,
                getBackend<VulkanBackend>()->mainCommandBuffer
            );
            stagingBuffer.destroy(getBackend<VulkanBackend>()->logicalDevice);
        }

        void VulkanLScene::buildModelIndexBuffer()
        {
            VulkanBufferCreateInfo createInfo;
            createInfo.physicalDevice   = getBackend<VulkanBackend>()->physicalDevice;
            createInfo.logicalDevice    = getBackend<VulkanBackend>()->logicalDevice;
            createInfo.size             = sizeof(Vertex::Index) * 2000000;
            createInfo.usage            = vk::BufferUsageFlagBits::eTransferDst | vk::BufferUsageFlagBits::eIndexBuffer;
            createInfo.memoryProperties = vk::MemoryPropertyFlagBits::eDeviceLocal;

            modelIndexBuffer.init(createInfo);
        }

        void VulkanLScene::buildModelIndexData(const Vertex::Indices& inIndices)
        {
            VulkanBufferCreateInfo createInfo;
            createInfo.physicalDevice = getBackend<VulkanBackend>()->physicalDevice;
            createInfo.logicalDevice  = getBackend<VulkanBackend>()->logicalDevice;
            createInfo.size           = sizeof(Vertex::Index) * inIndices.size();
            createInfo.usage          = vk::BufferUsageFlagBits::eTransferSrc;
            createInfo.memoryProperties =
                vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent;

            VulkanBuffer stagingBuffer;
            stagingBuffer.init(createInfo);

            void* writeLocation =
                getBackend<VulkanBackend>()->logicalDevice.mapMemory(stagingBuffer.memory, 0, createInfo.size);
            memcpy(writeLocation, inIndices.data(), createInfo.size);
            getBackend<VulkanBackend>()->logicalDevice.unmapMemory(stagingBuffer.memory);

            stagingBuffer.copy(
                modelIndexBuffer,
                createInfo.size,
                getBackend<VulkanBackend>()->graphicsQueue,
                getBackend<VulkanBackend>()->mainCommandBuffer
            );
            stagingBuffer.destroy(getBackend<VulkanBackend>()->logicalDevice);
        }

        void VulkanLScene::destroyModelData()
        {
            getBackend<VulkanBackend>()->logicalDevice.waitIdle();

            modelVertexBuffer.destroy(getBackend<VulkanBackend>()->logicalDevice);
            modelIndexBuffer.destroy(getBackend<VulkanBackend>()->logicalDevice);
        }

        void VulkanLScene::buildLayers()
        {
            addLayer<VulkanLSceneSky>();
            addLayer<VulkanLSceneShadow>();
            addLayer<VulkanLSceneMesh>();
        }
    }
}