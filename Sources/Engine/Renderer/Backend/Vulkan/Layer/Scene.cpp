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
            destroyTextureData();
            destroyModelData();
        }

        bool VulkanLScene::onInit()
        {
            buildTextureDescriptor();
            buildModelVertexBuffer();
            buildModelIndexBuffer();

            addLayer<VulkanLSceneSky>();
            addLayer<VulkanLSceneShadow>();
            addLayer<VulkanLSceneMesh>();

            return true;
        }

        void VulkanLScene::onLoad(const DrawTexture::List& inResources)
        {
            buildTextureData(inResources);
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

        void VulkanLScene::buildTextureDescriptor()
        {
            VulkanDescriptorSetLayoutBidingsCreateInfo layoutBidings;
            layoutBidings.count = 1;

            layoutBidings.indices.push_back(0);
            layoutBidings.types.push_back(vk::DescriptorType::eCombinedImageSampler);
            layoutBidings.counts.push_back(512);
            layoutBidings.stages.push_back(vk::ShaderStageFlagBits::eFragment);

            VulkanDescriptorSetLayout::init(
                textureDescriptor.setLayout,
                getBackend<VulkanBackend>()->logicalDevice,
                layoutBidings
            );

            VulkanDescriptorPoolCreateInfo descriptorPoolCreateInfo;
            descriptorPoolCreateInfo.maxSets = 1;
            descriptorPoolCreateInfo.sizes.push_back({vk::DescriptorType::eCombinedImageSampler, 512});

            VulkanDescriptorPool::init(
                textureDescriptor.pool,
                getBackend<VulkanBackend>()->logicalDevice,
                descriptorPoolCreateInfo
            );

            VulkanDescriptorSetLayout::allocate(
                textureDescriptor.set,
                getBackend<VulkanBackend>()->logicalDevice,
                textureDescriptor.setLayout,
                textureDescriptor.pool
            );
        }

        void VulkanLScene::buildTextureData(const DrawTexture::List& inTextures)
        {
            if (inTextures.empty())
            {
                return;
            }

            VulkanTextureCreateInfo createInfo;
            createInfo.logicalDevice  = getBackend<VulkanBackend>()->logicalDevice;
            createInfo.physicalDevice = getBackend<VulkanBackend>()->physicalDevice;
            createInfo.commandBuffer  = getBackend<VulkanBackend>()->mainCommandBuffer;
            createInfo.queue          = getBackend<VulkanBackend>()->graphicsQueue;

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

            getBackend<VulkanBackend>()->logicalDevice.updateDescriptorSets(set, nullptr);
        }

        void VulkanLScene::destroyTextureData()
        {
            textures.clear();

            getBackend<VulkanBackend>()->logicalDevice.destroyDescriptorSetLayout(textureDescriptor.setLayout);
            getBackend<VulkanBackend>()->logicalDevice.destroyDescriptorPool(textureDescriptor.pool);
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
    }
}