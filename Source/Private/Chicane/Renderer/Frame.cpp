#include "Chicane/Renderer/Frame.hpp"

#include "Chicane/Core/Loader.hpp"
#include "Chicane/Game/Actor/Component/Mesh.hpp"

namespace Chicane
{
    namespace Frame
    {
        bool Instance::isDirty()
        {
            return m_bIsDirty;
        }

        void Instance::setAsDirty()
        {
            m_bIsDirty = true;
        }

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

        void Instance::setupMeshData(const std::vector<MeshComponent*>& inMeshes)
        {
            destroyMeshData();

            if (inMeshes.empty())
            {
                return;
            }

            Chicane::Buffer::CreateInfo bufferCreateInfo;
            bufferCreateInfo.logicalDevice    = logicalDevice;
            bufferCreateInfo.physicalDevice   = physicalDevice;
            bufferCreateInfo.memoryProperties = vk::MemoryPropertyFlagBits::eHostVisible |
                                                     vk::MemoryPropertyFlagBits::eHostCoherent;
            bufferCreateInfo.size             = sizeof(Mesh::Data) * inMeshes.size();
            bufferCreateInfo.usage            = vk::BufferUsageFlagBits::eStorageBuffer;

            Buffer::init(meshBundle.buffer, bufferCreateInfo);
        
            meshBundle.allocationSize = bufferCreateInfo.size;
            meshBundle.writeLocation  = logicalDevice.mapMemory(
                meshBundle.buffer.memory,
                0,
                meshBundle.allocationSize
            );
            modelDescriptorBufferInfo.buffer = meshBundle.buffer.instance;
            modelDescriptorBufferInfo.offset = 0;
            modelDescriptorBufferInfo.range  = meshBundle.allocationSize;

            meshBundle.data.clear();
            meshBundle.data.reserve(inMeshes.size());
            for (const MeshComponent* mesh : inMeshes)
            {
                Mesh::Data data {};
                data.transform    = mesh->getPosition();
                data.textureIndex = Vec<4, float>(
                    Loader::getTextureManager()->getIndex(mesh->getTexture())
                );

                meshBundle.data.push_back(data);
            }

            memcpy(
                meshBundle.writeLocation,
                meshBundle.data.data(),
                meshBundle.allocationSize
            );
        }

        void Instance::updateMeshData(const std::vector<MeshComponent*>& inMeshes)
        {
            if (inMeshes.empty())
            {
                destroyMeshData();

                return;
            }

            if (isDirty())
            {
                setupMeshData(inMeshes);

                m_bIsDirty = false;

                return;
            }

            for (std::uint32_t i = 0; i < inMeshes.size(); i++)
            {
                meshBundle.data.at(i).transform = inMeshes.at(i)->getPosition();
            }

            memcpy(
                meshBundle.writeLocation,
                meshBundle.data.data(),
                meshBundle.allocationSize
            );
        }
    
        void Instance::destroyMeshData()
        {
            if (!meshBundle.buffer.memory)
            {
                return;
            }

            logicalDevice.unmapMemory(meshBundle.buffer.memory);
            Buffer::destroy(
                logicalDevice,
                meshBundle.buffer
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
            destroyMeshData();

            logicalDevice.destroyImageView(imageView);
        }
    }
}