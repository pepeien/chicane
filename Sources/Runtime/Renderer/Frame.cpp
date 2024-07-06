#include "Runtime/Renderer/Frame.hpp"

namespace Chicane
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
            Chicane::Buffer::CreateInfo bufferCreateInfo;
            bufferCreateInfo.logicalDevice    = logicalDevice;
            bufferCreateInfo.physicalDevice   = physicalDevice;
            bufferCreateInfo.memoryProperties = vk::MemoryPropertyFlagBits::eHostVisible |
                                                vk::MemoryPropertyFlagBits::eHostCoherent;
            bufferCreateInfo.size             = sizeof(VectorUBO);
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
            Chicane::Buffer::CreateInfo bufferCreateInfo;
            bufferCreateInfo.logicalDevice    = logicalDevice;
            bufferCreateInfo.physicalDevice   = physicalDevice;
            bufferCreateInfo.memoryProperties = vk::MemoryPropertyFlagBits::eHostVisible |
                                                vk::MemoryPropertyFlagBits::eHostCoherent;
            bufferCreateInfo.size             = sizeof(MatrixUBO);
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
            Chicane::Buffer::CreateInfo modelBufferCreateInfo;
            modelBufferCreateInfo.logicalDevice    = logicalDevice;
            modelBufferCreateInfo.physicalDevice   = physicalDevice;
            modelBufferCreateInfo.memoryProperties = vk::MemoryPropertyFlagBits::eHostVisible |
                                                     vk::MemoryPropertyFlagBits::eHostCoherent;
            modelBufferCreateInfo.size             = sizeof(glm::mat4) * inActors.size();
            modelBufferCreateInfo.usage            = vk::BufferUsageFlagBits::eStorageBuffer;
    
            Buffer::init(meshData.buffer, modelBufferCreateInfo);
        
            meshData.allocationSize = modelBufferCreateInfo.size;
            meshData.writeLocation  = logicalDevice.mapMemory(
                meshData.buffer.memory,
                0,
                meshData.allocationSize
            );
            meshData.transforms.reserve(inActors.size());
    
            for (Actor* actor : inActors)
            {
                meshData.transforms.push_back(actor->getPosition());
            }
            
            modelDescriptorBufferInfo.buffer = meshData.buffer.instance;
            modelDescriptorBufferInfo.offset = 0;
            modelDescriptorBufferInfo.range  = meshData.allocationSize;
        }

        void Instance::updateModelData(std::vector<Actor*> inActors)
        {   
            for (uint32_t i = 0; i < inActors.size() && i < meshData.transforms.size(); i++)
            {
                meshData.transforms[i] = inActors[i]->getPosition();
            }
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

        void Instance::addFrameBuffer(const std::string& inId, const vk::Framebuffer& inFramebuffer)
        {
            if (framebuffers.find(inId) != framebuffers.end())
            {
                throw std::runtime_error("Framebuffer " + inId + " already exists");
            }

            framebuffers.insert(std::make_pair(inId, inFramebuffer));
        }

        vk::Framebuffer Instance::getFramebuffer(const std::string& inId)
        {
            return framebuffers.at(inId);
        }

        void Instance::addDescriptorSet(const std::string& inId, const vk::DescriptorSet& inDescriptorSet)
        {
            if (descriptorSets.find(inId) != descriptorSets.end())
            {
                throw std::runtime_error("Descriptor set " + inId + " already exists");
            }

            descriptorSets.insert(std::make_pair(inId, inDescriptorSet));
        }

        vk::DescriptorSet Instance::getDescriptorSet(const std::string& inId)
        {
            return descriptorSets.at(inId);
        }

        void Instance::addWriteDescriptorSet(const vk::WriteDescriptorSet& inWriteDescriptorSet)
        {
            descriptorSetWrites.push_back(inWriteDescriptorSet);
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
            for (auto& [id, frambuffer] : framebuffers)
            {
                logicalDevice.destroyFramebuffer(frambuffer);
            }

            logicalDevice.destroyImageView(imageView);

            logicalDevice.freeMemory(depthMemory);
            logicalDevice.destroyImage(depthImage);
            logicalDevice.destroyImageView(depthImageView);

            logicalDevice.destroyFence(renderFence);
            logicalDevice.destroySemaphore(presentSemaphore);
            logicalDevice.destroySemaphore(renderSemaphore);

            if (cameraMatrixUBO.buffer.memory)
            { 
                logicalDevice.unmapMemory(cameraMatrixUBO.buffer.memory);
                Buffer::destroy(
                    logicalDevice,
                    cameraMatrixUBO.buffer
                );
            }

            if (cameraVectorUBO.buffer.memory)
            {
                logicalDevice.unmapMemory(cameraVectorUBO.buffer.memory);
                Buffer::destroy(
                    logicalDevice,
                    cameraVectorUBO.buffer
                );
            }

            if (meshData.buffer.memory)
            {
                logicalDevice.unmapMemory(meshData.buffer.memory);
                Buffer::destroy(
                    logicalDevice,
                    meshData.buffer
                );
            }
        }
    }
}