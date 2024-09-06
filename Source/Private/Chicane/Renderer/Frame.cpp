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

        void Instance::wait(const vk::Device& inLogicalDevice)
        {
            vk::Result result = inLogicalDevice.waitForFences(
                1,
                &renderFence,
                VK_TRUE,
                UINT64_MAX
            );

            if (result != vk::Result::eSuccess && result != vk::Result::eTimeout)
            {
                throw std::runtime_error("Error while waiting the fences");
            }
        }

        void Instance::reset(const vk::Device& inLogicalDevice)
        {
            vk::Result result = inLogicalDevice.resetFences(
                1,
                &renderFence
            );

            if (result != vk::Result::eSuccess)
            {
                throw std::runtime_error("Error while resetting the fences");
            } 
        }

        vk::ResultValue<std::uint32_t> Instance::getNextIndex(
            const vk::SwapchainKHR& inSwapchain,
            const vk::Device& inLogicalDevice
        )
        {
            vk::ResultValue<std::uint32_t> result = inLogicalDevice.acquireNextImageKHR(
                inSwapchain,
                UINT64_MAX,
                presentSemaphore,
                nullptr
            );

            return result;
        }

        void Instance::destroySync()
        {
            logicalDevice.destroyFence(renderFence);
            logicalDevice.destroySemaphore(presentSemaphore);
            logicalDevice.destroySemaphore(renderSemaphore);
        }

        void Instance::setupCameraUBO()
        {
            Chicane::Buffer::CreateInfo bufferCreateInfo;
            bufferCreateInfo.logicalDevice    = logicalDevice;
            bufferCreateInfo.physicalDevice   = physicalDevice;
            bufferCreateInfo.memoryProperties = vk::MemoryPropertyFlagBits::eHostVisible |
                                                vk::MemoryPropertyFlagBits::eHostCoherent;
            bufferCreateInfo.size             = sizeof(Camera::UBO);
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

        void Instance::destroyCameraUBO()
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

        void Instance::setupModelData(Level* inLevel)
        {
            const std::vector<MeshComponent*> meshes = inLevel->getMeshes();

            Chicane::Buffer::CreateInfo modelBufferCreateInfo;
            modelBufferCreateInfo.logicalDevice    = logicalDevice;
            modelBufferCreateInfo.physicalDevice   = physicalDevice;
            modelBufferCreateInfo.memoryProperties = vk::MemoryPropertyFlagBits::eHostVisible |
                                                     vk::MemoryPropertyFlagBits::eHostCoherent;
            modelBufferCreateInfo.size             = sizeof(Mat<4, float>) * meshes.size();
            modelBufferCreateInfo.usage            = vk::BufferUsageFlagBits::eStorageBuffer;
    
            Buffer::init(modelData.buffer, modelBufferCreateInfo);
        
            modelData.allocationSize = modelBufferCreateInfo.size;
            modelData.writeLocation  = logicalDevice.mapMemory(
                modelData.buffer.memory,
                0,
                modelData.allocationSize
            );

            for (const MeshComponent* mesh : meshes)
            {
                if (!mesh->isDrawable())
                {
                    continue;
                }

                modelData.transforms.push_back(mesh->getPosition());
            }

            modelDescriptorBufferInfo.buffer = modelData.buffer.instance;
            modelDescriptorBufferInfo.offset = 0;
            modelDescriptorBufferInfo.range  = modelData.allocationSize;
        }

        void Instance::updateModelData(Level* inLevel)
        {
            const std::vector<MeshComponent*> meshes = inLevel->getMeshes();

            for (uint32_t i = 0; i < meshes.size(); i++)
            {
                modelData.transforms[i] = meshes.at(i)->getPosition();
            }
        }
    
        void Instance::destroyModelData()
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

        void Instance::setupDepthBuffering(const vk::Format& inFormat)
        {
            depth.format = inFormat;

            Image::CreateInfo imageCreateInfo;
            imageCreateInfo.width            = width;
            imageCreateInfo.height           = height;
            imageCreateInfo.count            = 1;
            imageCreateInfo.physicalDevice   = physicalDevice;
            imageCreateInfo.logicalDevice    = logicalDevice;
            imageCreateInfo.tiling           = vk::ImageTiling::eOptimal;
            imageCreateInfo.usage            = vk::ImageUsageFlagBits::eDepthStencilAttachment;
            imageCreateInfo.memoryProperties = vk::MemoryPropertyFlagBits::eDeviceLocal;
            imageCreateInfo.format           = depth.format;

            Image::init(depth.image, imageCreateInfo);
            Image::initMemory(
                depth.memory,
                imageCreateInfo,
                depth.image
            );
            Image::initView(
                depth.imageView,
                logicalDevice,
                depth.image,
                depth.format,
                vk::ImageAspectFlagBits::eDepth,
                vk::ImageViewType::e2D,
                1
            );
        }

        void Instance::destroyDepthBuffering()
        {
            logicalDevice.freeMemory(depth.memory);
            logicalDevice.destroyImage(depth.image);
            logicalDevice.destroyImageView(depth.imageView);
        }

        void Instance::addFrameBuffer(const std::string& inId, const vk::Framebuffer& inFramebuffer)
        {
            if (framebuffers.find(inId) != framebuffers.end())
            {
                throw std::runtime_error("Framebuffer " + inId + " already exists");
            }

            framebuffers.insert(std::make_pair(inId, inFramebuffer));
        }

        vk::Framebuffer Instance::getFramebuffer(const std::string& inId) const
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

        vk::DescriptorSet Instance::getDescriptorSet(const std::string& inId) const
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

            destroyDepthBuffering();
            destroySync();
            destroyCameraUBO();
            destroyModelData();

            logicalDevice.destroyImageView(imageView);
        }
    }
}