#include "Chicane/Renderer/Frame.hpp"

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
            
        void Instance::setupCameraUBO()
        {
            Chicane::Buffer::CreateInfo bufferCreateInfo;
            bufferCreateInfo.logicalDevice    = logicalDevice;
            bufferCreateInfo.physicalDevice   = physicalDevice;
            bufferCreateInfo.memoryProperties = vk::MemoryPropertyFlagBits::eHostVisible |
                                                vk::MemoryPropertyFlagBits::eHostCoherent;
            bufferCreateInfo.size             = sizeof(UBO);
            bufferCreateInfo.usage            = vk::BufferUsageFlagBits::eUniformBuffer;

            Buffer::init(
                cameraUBO.buffer,
                bufferCreateInfo
            );

            cameraUBO.allocationSize = bufferCreateInfo.size;
            cameraUBO.writeLocation  = logicalDevice.mapMemory(
                cameraUBO.buffer.memory,
                0,
                cameraUBO.allocationSize
            );

            cameraDescriptorBufferInfo.buffer = cameraUBO.buffer.instance;
            cameraDescriptorBufferInfo.offset = 0;
            cameraDescriptorBufferInfo.range  = cameraUBO.allocationSize;
        }
    
        void Instance::setupModelData(const std::vector<Actor*>& inActors)
        {
            destroyModelMemory();

            Chicane::Buffer::CreateInfo modelBufferCreateInfo;
            modelBufferCreateInfo.logicalDevice    = logicalDevice;
            modelBufferCreateInfo.physicalDevice   = physicalDevice;
            modelBufferCreateInfo.memoryProperties = vk::MemoryPropertyFlagBits::eHostVisible |
                                                     vk::MemoryPropertyFlagBits::eHostCoherent;
            modelBufferCreateInfo.size             = sizeof(Mat<float>::Four) * inActors.size();
            modelBufferCreateInfo.usage            = vk::BufferUsageFlagBits::eStorageBuffer;
    
            Buffer::init(modelData.buffer, modelBufferCreateInfo);
        
            modelData.allocationSize = modelBufferCreateInfo.size;
            modelData.writeLocation  = logicalDevice.mapMemory(
                modelData.buffer.memory,
                0,
                modelData.allocationSize
            );
            modelData.transforms.reserve(inActors.size());
    
            for (Actor* actor : inActors)
            {
                modelData.transforms.push_back(actor->getPosition());
            }
            
            modelDescriptorBufferInfo.buffer = modelData.buffer.instance;
            modelDescriptorBufferInfo.offset = 0;
            modelDescriptorBufferInfo.range  = modelData.allocationSize;
        }

        void Instance::updateModelData(const std::vector<Actor*>& inActors)
        {   
            for (uint32_t i = 0; i < inActors.size() && i < modelData.transforms.size(); i++)
            {
                modelData.transforms[i] = inActors[i]->getPosition();
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


        void Instance::destroyCameraMemory()
        {
            if (!cameraUBO.buffer.memory)
            { 
                return;
            }

            logicalDevice.unmapMemory(cameraUBO.buffer.memory);
            Buffer::destroy(
                logicalDevice,
                cameraUBO.buffer
            );
        }
    
        void Instance::destroyModelMemory()
        {
            if (!modelData.buffer.memory)
            {
                return;
            }

            logicalDevice.unmapMemory(modelData.buffer.memory);
            Buffer::destroy(
                logicalDevice,
                modelData.buffer
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

            destroyCameraMemory();
            destroyModelMemory();
        }
    }
}