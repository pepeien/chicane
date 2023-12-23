#include "Instance.hpp"

namespace Chicane
{
    namespace Frame
    {
        void Instance::setupSync()
        {
            Sync::initSempahore(presentSemaphore, logicalDevice);
            Sync::initSempahore(renderSemaphore, logicalDevice);
            Sync::initFence(renderFence, logicalDevice);
        }
            
        void Instance::setupCamera()
        {
            Chicane::Buffer::CreateInfo cameraBufferCreateInfo;
            cameraBufferCreateInfo.logicalDevice    = logicalDevice;
            cameraBufferCreateInfo.physicalDevice   = physicalDevice;
            cameraBufferCreateInfo.memoryProperties = vk::MemoryPropertyFlagBits::eHostVisible |
                                                      vk::MemoryPropertyFlagBits::eHostCoherent;
            cameraBufferCreateInfo.size             = sizeof(Uniform::BufferObject);
            cameraBufferCreateInfo.usage            = vk::BufferUsageFlagBits::eUniformBuffer;
    
            Buffer::init(cameraData.buffer, cameraBufferCreateInfo);
    
            cameraData.allocationSize = cameraBufferCreateInfo.size;
            cameraData.writeLocation  = logicalDevice.mapMemory(
                cameraData.buffer.memory,
                0,
                cameraData.allocationSize
            );
    
            uniformDescriptorBufferInfo.buffer = cameraData.buffer.instance;
            uniformDescriptorBufferInfo.offset = 0;
            uniformDescriptorBufferInfo.range  = cameraData.allocationSize;
        }
    
        void Instance::setupModel(const Scene::Instance& inScene)
        {
            auto sceneObjects = inScene.getObjects();
    
            Chicane::Buffer::CreateInfo modelBufferCreateInfo;
            modelBufferCreateInfo.logicalDevice    = logicalDevice;
            modelBufferCreateInfo.physicalDevice   = physicalDevice;
            modelBufferCreateInfo.memoryProperties = vk::MemoryPropertyFlagBits::eHostVisible |
                                                     vk::MemoryPropertyFlagBits::eHostCoherent;
            modelBufferCreateInfo.size             = sizeof(glm::mat4) * sceneObjects.size();
            modelBufferCreateInfo.usage            = vk::BufferUsageFlagBits::eStorageBuffer;
    
            Buffer::init(modelData.buffer, modelBufferCreateInfo);
        
            modelData.allocationSize = modelBufferCreateInfo.size;
            modelData.writeLocation  = logicalDevice.mapMemory(
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

        void Instance::setupDescriptors(
            const vk::DescriptorSetLayout& inLayout,
            const vk::DescriptorPool& inPool
        )
        {
            Descriptor::initSet(
                descriptorSet,
                logicalDevice,
                inLayout,
                inPool
            );
        }
        
        void Instance::setupDepthBuffering()
        {
            depthFormat = Image::findSupportedFormat(
                physicalDevice,
                { vk::Format::eD32Sfloat, vk::Format::eD24UnormS8Uint },
                vk::ImageTiling::eOptimal,
                vk::FormatFeatureFlagBits::eDepthStencilAttachment
            );

            Image::CreateInfo depthImageCreateInfo;
            depthImageCreateInfo.physicalDevice   = physicalDevice;
            depthImageCreateInfo.logicalDevice    = logicalDevice;
            depthImageCreateInfo.tiling           = vk::ImageTiling::eOptimal;
            depthImageCreateInfo.usage            = vk::ImageUsageFlagBits::eDepthStencilAttachment;
            depthImageCreateInfo.memoryProperties = vk::MemoryPropertyFlagBits::eDeviceLocal;
            depthImageCreateInfo.width            = width;
            depthImageCreateInfo.height           = height;
            depthImageCreateInfo.format           = depthFormat;

            Image::init(depthImage, depthImageCreateInfo);
            Image::initMemory(
                depthMemory,
                depthImageCreateInfo,
                depthImage
            );
            Image::initView(
                depthImageView,
                logicalDevice,
                depthImage,
                depthFormat,
                vk::ImageAspectFlagBits::eDepth
            );
        }
    
        void Instance::setupDescriptorSet()
        {
            vk::WriteDescriptorSet uniformWriteInfo;
            uniformWriteInfo.dstSet          = descriptorSet;
            uniformWriteInfo.dstBinding      = 0;
            uniformWriteInfo.dstArrayElement = 0;
            uniformWriteInfo.descriptorCount = 1;
            uniformWriteInfo.descriptorType  = vk::DescriptorType::eUniformBuffer;
            uniformWriteInfo.pBufferInfo     = &uniformDescriptorBufferInfo;

            logicalDevice.updateDescriptorSets(
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

            logicalDevice.updateDescriptorSets(
                modelWriteInfo,
                nullptr
            );
        }

        void Instance::destroy()
        {
            logicalDevice.destroyImageView(imageView);
            logicalDevice.destroyFramebuffer(framebuffer);

            logicalDevice.destroyImage(depthImage);
            logicalDevice.freeMemory(depthMemory);
            logicalDevice.destroyImageView(depthImageView);

            logicalDevice.destroyFence(renderFence);
            logicalDevice.destroySemaphore(presentSemaphore);
            logicalDevice.destroySemaphore(renderSemaphore);

            logicalDevice.unmapMemory(cameraData.buffer.memory);
            Buffer::destroy(logicalDevice, cameraData.buffer);

            logicalDevice.unmapMemory(modelData.buffer.memory);
            Buffer::destroy(logicalDevice, modelData.buffer);
        }
    }
}