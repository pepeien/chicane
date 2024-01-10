#include "Instance.hpp"

namespace Engine
{
    namespace Frame
    {
        void Instance::setupSync()
        {
            Sync::initSempahore(
                presentSemaphore,
                logicalDevice
            );
            Sync::initSempahore(
                renderSemaphore,
                logicalDevice
            );
            Sync::initFence(
                renderFence,
                logicalDevice
            );
        }

        void Instance::setupCameraVectorUBO()
        {
            Engine::Buffer::CreateInfo bufferCreateInfo;
            bufferCreateInfo.logicalDevice    = logicalDevice;
            bufferCreateInfo.physicalDevice   = physicalDevice;
            bufferCreateInfo.memoryProperties = vk::MemoryPropertyFlagBits::eHostVisible |
                                                vk::MemoryPropertyFlagBits::eHostCoherent;
            bufferCreateInfo.size             = sizeof(Camera::VectorUBO);
            bufferCreateInfo.usage            = vk::BufferUsageFlagBits::eUniformBuffer;

            Buffer::init(
                cameraVectorUBO.buffer,
                bufferCreateInfo
            );

            cameraVectorUBO.allocationSize = bufferCreateInfo.size;
            cameraVectorUBO.writeLocation  = logicalDevice.mapMemory(
                cameraVectorUBO.buffer.memory,
                0,
                cameraVectorUBO.allocationSize
            );

            cameraVectorDescriptorBufferInfo.buffer = cameraVectorUBO.buffer.instance;
            cameraVectorDescriptorBufferInfo.offset = 0;
            cameraVectorDescriptorBufferInfo.range  = cameraVectorUBO.allocationSize;
        }
            
        void Instance::setupCameraMatrixUBO()
        {
            Engine::Buffer::CreateInfo bufferCreateInfo;
            bufferCreateInfo.logicalDevice    = logicalDevice;
            bufferCreateInfo.physicalDevice   = physicalDevice;
            bufferCreateInfo.memoryProperties = vk::MemoryPropertyFlagBits::eHostVisible |
                                                vk::MemoryPropertyFlagBits::eHostCoherent;
            bufferCreateInfo.size             = sizeof(Camera::MatrixUBO);
            bufferCreateInfo.usage            = vk::BufferUsageFlagBits::eUniformBuffer;

            Buffer::init(
                cameraMatrixUBO.buffer,
                bufferCreateInfo
            );

            cameraMatrixUBO.allocationSize = bufferCreateInfo.size;
            cameraMatrixUBO.writeLocation  = logicalDevice.mapMemory(
                cameraMatrixUBO.buffer.memory,
                0,
                cameraMatrixUBO.allocationSize
            );

            cameraMatrixDescriptorBufferInfo.buffer = cameraMatrixUBO.buffer.instance;
            cameraMatrixDescriptorBufferInfo.offset = 0;
            cameraMatrixDescriptorBufferInfo.range  = cameraMatrixUBO.allocationSize;
        }
    
        void Instance::setupModelData(std::vector<Actor*> inActors)
        {
            Engine::Buffer::CreateInfo modelBufferCreateInfo;
            modelBufferCreateInfo.logicalDevice    = logicalDevice;
            modelBufferCreateInfo.physicalDevice   = physicalDevice;
            modelBufferCreateInfo.memoryProperties = vk::MemoryPropertyFlagBits::eHostVisible |
                                                     vk::MemoryPropertyFlagBits::eHostCoherent;
            modelBufferCreateInfo.size             = sizeof(glm::mat4) * inActors.size();
            modelBufferCreateInfo.usage            = vk::BufferUsageFlagBits::eStorageBuffer;
    
            Buffer::init(modelData.buffer, modelBufferCreateInfo);
        
            modelData.allocationSize = modelBufferCreateInfo.size;
            modelData.writeLocation  = logicalDevice.mapMemory(
                modelData.buffer.memory,
                0,
                modelData.allocationSize
            );
            modelData.transforms.reserve(inActors.size());
    
            for (auto actor : inActors)
            {
                modelData.transforms.push_back(actor->getPosition());
            }
            
            modelDescriptorBufferInfo.buffer = modelData.buffer.instance;
            modelDescriptorBufferInfo.offset = 0;
            modelDescriptorBufferInfo.range  = modelData.allocationSize;
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
            depthImageCreateInfo.width            = width;
            depthImageCreateInfo.height           = height;
            depthImageCreateInfo.count            = 1;
            depthImageCreateInfo.physicalDevice   = physicalDevice;
            depthImageCreateInfo.logicalDevice    = logicalDevice;
            depthImageCreateInfo.tiling           = vk::ImageTiling::eOptimal;
            depthImageCreateInfo.usage            = vk::ImageUsageFlagBits::eDepthStencilAttachment;
            depthImageCreateInfo.memoryProperties = vk::MemoryPropertyFlagBits::eDeviceLocal;
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
                vk::ImageAspectFlagBits::eDepth,
                vk::ImageViewType::e2D,
                1
            );
        }
    
        void Instance::addDescriptorSet(
            GraphicsPipeline::Type inType,
            const vk::DescriptorSetLayout& inLayout,
            const vk::DescriptorPool& inPool
        )
        {
            Descriptor::initSet(
                descriptorSets[inType],
                logicalDevice,
                inLayout,
                inPool
            );
        }

        void Instance::setupDescriptorSets()
        {
            // Sky
            vk::WriteDescriptorSet cameraVectorWriteInfo;
            cameraVectorWriteInfo.dstSet          = descriptorSets.at(GraphicsPipeline::Type::SKY);
            cameraVectorWriteInfo.dstBinding      = 0;
            cameraVectorWriteInfo.dstArrayElement = 0;
            cameraVectorWriteInfo.descriptorCount = 1;
            cameraVectorWriteInfo.descriptorType  = vk::DescriptorType::eUniformBuffer;
            cameraVectorWriteInfo.pBufferInfo     = &cameraVectorDescriptorBufferInfo;

            // Scene
            vk::WriteDescriptorSet cameraMatrixWriteInfo;
            cameraMatrixWriteInfo.dstSet          = descriptorSets.at(GraphicsPipeline::Type::SCENE);
            cameraMatrixWriteInfo.dstBinding      = 0;
            cameraMatrixWriteInfo.dstArrayElement = 0;
            cameraMatrixWriteInfo.descriptorCount = 1;
            cameraMatrixWriteInfo.descriptorType  = vk::DescriptorType::eUniformBuffer;
            cameraMatrixWriteInfo.pBufferInfo     = &cameraMatrixDescriptorBufferInfo;

            vk::WriteDescriptorSet modelWriteInfo;
            modelWriteInfo.dstSet          = descriptorSets.at(GraphicsPipeline::Type::SCENE);
            modelWriteInfo.dstBinding      = 1;
            modelWriteInfo.dstArrayElement = 0;
            modelWriteInfo.descriptorCount = 1;
            modelWriteInfo.descriptorType  = vk::DescriptorType::eStorageBuffer;
            modelWriteInfo.pBufferInfo     = &modelDescriptorBufferInfo;

            descriptorSetWrites = {
                cameraVectorWriteInfo,
                cameraMatrixWriteInfo,
                modelWriteInfo
            };
        }

        void Instance::updateDescriptorSets()
        {
            logicalDevice.updateDescriptorSets(
                descriptorSetWrites,
                nullptr
            );
        }

        void Instance::destroy()
        {
            logicalDevice.destroyImageView(imageView);

            for (auto& [type, instance] : framebuffers)
            {
                logicalDevice.destroyFramebuffer(instance);
            }

            logicalDevice.destroyImage(depthImage);
            logicalDevice.freeMemory(depthMemory);
            logicalDevice.destroyImageView(depthImageView);

            logicalDevice.destroyFence(renderFence);
            logicalDevice.destroySemaphore(presentSemaphore);
            logicalDevice.destroySemaphore(renderSemaphore);

            logicalDevice.unmapMemory(cameraMatrixUBO.buffer.memory);
            Buffer::destroy(
                logicalDevice,
                cameraMatrixUBO.buffer
            );

            logicalDevice.unmapMemory(cameraVectorUBO.buffer.memory);
            Buffer::destroy(
                logicalDevice,
                cameraVectorUBO.buffer
            );

            logicalDevice.unmapMemory(modelData.buffer.memory);
            Buffer::destroy(
                logicalDevice,
                modelData.buffer
            );
        }
    }
}