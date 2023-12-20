#include "Instance.hpp"

namespace Engine
{
    namespace Frame
    {
        void Instance::initResources(
            const vk::Device& inLogicalDevice,
            const vk::PhysicalDevice& inPhysicalDevice,
            const Scene::Instance& inScene
        )
        {
            createCameraData(inLogicalDevice, inPhysicalDevice);
            createModelData(inLogicalDevice, inPhysicalDevice, inScene);
        }
    
        void Instance::writeDescriptorSet(const vk::Device& inLogicalDevice)
        {
            vk::WriteDescriptorSet uniformWriteInfo;
            uniformWriteInfo.dstSet          = descriptorSet;
            uniformWriteInfo.dstBinding      = 0;
            uniformWriteInfo.dstArrayElement = 0;
            uniformWriteInfo.descriptorCount = 1;
            uniformWriteInfo.descriptorType  = vk::DescriptorType::eUniformBuffer;
            uniformWriteInfo.pBufferInfo     = &uniformDescriptorBufferInfo;
    
            inLogicalDevice.updateDescriptorSets(
                uniformWriteInfo,
                nullptr
            );
    
            vk::WriteDescriptorSet modelWriteInfo;
            modelWriteInfo.dstSet          = descriptorSet;
            modelWriteInfo.dstBinding      = 1;
            modelWriteInfo.dstArrayElement = 0;
            modelWriteInfo.descriptorCount = 1;
            modelWriteInfo.descriptorType  = vk::DescriptorType::eStorageBuffer;
            modelWriteInfo.pBufferInfo     = &modelDescriptorBufferInfo;
    
            inLogicalDevice.updateDescriptorSets(
                modelWriteInfo,
                nullptr
            );
        }
    
        void Instance::createCameraData(
            const vk::Device& inLogicalDevice,
            const vk::PhysicalDevice& inPhysicalDevice
        )
        {
            Buffer::CreateInfo cameraBufferCreateInfo;
            cameraBufferCreateInfo.logicalDevice    = inLogicalDevice;
            cameraBufferCreateInfo.physicalDevice   = inPhysicalDevice;
            cameraBufferCreateInfo.memoryProperties = vk::MemoryPropertyFlagBits::eHostVisible |
                                                      vk::MemoryPropertyFlagBits::eHostCoherent;
            cameraBufferCreateInfo.size             = sizeof(Uniform::BufferObject);
            cameraBufferCreateInfo.usage            = vk::BufferUsageFlagBits::eUniformBuffer;
    
            Buffer::init(cameraData.buffer, cameraBufferCreateInfo);
    
            cameraData.allocationSize = cameraBufferCreateInfo.size;
            cameraData.writeLocation  = inLogicalDevice.mapMemory(
                cameraData.buffer.memory,
                0,
                cameraData.allocationSize
            );
    
            uniformDescriptorBufferInfo.buffer = cameraData.buffer.instance;
            uniformDescriptorBufferInfo.offset = 0;
            uniformDescriptorBufferInfo.range  = cameraData.allocationSize;
        }
    
        void Instance::createModelData(
            const vk::Device& inLogicalDevice,
            const vk::PhysicalDevice& inPhysicalDevice,
            const Scene::Instance& inScene
        )
        {
            auto sceneObjects = inScene.getObjects();
    
            Buffer::CreateInfo modelBufferCreateInfo;
            modelBufferCreateInfo.logicalDevice    = inLogicalDevice;
            modelBufferCreateInfo.physicalDevice   = inPhysicalDevice;
            modelBufferCreateInfo.memoryProperties = vk::MemoryPropertyFlagBits::eHostVisible |
                                                     vk::MemoryPropertyFlagBits::eHostCoherent;
            modelBufferCreateInfo.size             = sizeof(glm::mat4) * sceneObjects.size();
            modelBufferCreateInfo.usage            = vk::BufferUsageFlagBits::eStorageBuffer;
    
            Buffer::init(modelData.buffer, modelBufferCreateInfo);
        
            modelData.allocationSize = modelBufferCreateInfo.size;
            modelData.writeLocation  = inLogicalDevice.mapMemory(
                modelData.buffer.memory,
                0,
                modelData.allocationSize
            );
            modelData.transforms.reserve(sceneObjects.size());
    
            for (uint32_t i = 0; i < sceneObjects.size(); i++)
            {
                modelData.transforms.push_back(glm::mat4(1.0f));
            }
            
            modelDescriptorBufferInfo.buffer = modelData.buffer.instance;
            modelDescriptorBufferInfo.offset = 0;
            modelDescriptorBufferInfo.range  = modelData.allocationSize;
        }
    }
}