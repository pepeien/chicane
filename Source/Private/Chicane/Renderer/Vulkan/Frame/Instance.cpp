#include "Chicane/Renderer/Vulkan/Frame/Instance.hpp"

#include "Chicane/Core/Loader.hpp"
#include "Chicane/Game/Level/Instance.hpp"
#include "Chicane/Renderer/Vulkan/Buffer.hpp"
#include "Chicane/Renderer/Vulkan/Descriptor.hpp"
#include "Chicane/Renderer/Vulkan/GraphicsPipeline.hpp"
#include "Chicane/Renderer/Vulkan/Image.hpp"
#include "Chicane/Renderer/Vulkan/Sync.hpp"

namespace Chicane
{
    namespace Vulkan
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

            bool Instance::isCameraDirty()
            {
                return m_bIsCameraDirty;
            }

            void Instance::setCameraAsDirty()
            {
                m_bIsCameraDirty = true;
            }

            void Instance::setupCameraData(CameraComponent* inCamera)
            {
                Buffer::CreateInfo bufferCreateInfo {};
                bufferCreateInfo.logicalDevice    = logicalDevice;
                bufferCreateInfo.physicalDevice   = physicalDevice;
                bufferCreateInfo.memoryProperties = vk::MemoryPropertyFlagBits::eHostVisible |
                                                    vk::MemoryPropertyFlagBits::eHostCoherent;
                bufferCreateInfo.size             = sizeof(Chicane::Camera::Data);
                bufferCreateInfo.usage            = vk::BufferUsageFlagBits::eUniformBuffer;

                Buffer::init(m_cameraResource.buffer, bufferCreateInfo);

                m_cameraResource.allocationSize = bufferCreateInfo.size;
                m_cameraResource.writeLocation  = logicalDevice.mapMemory(
                    m_cameraResource.buffer.memory,
                    0,
                    m_cameraResource.allocationSize
                );

                cameraDescriptorBufferInfo.buffer = m_cameraResource.buffer.instance;
                cameraDescriptorBufferInfo.offset = 0;
                cameraDescriptorBufferInfo.range  = m_cameraResource.allocationSize;

                m_cameraResource.data = inCamera->getData();

                memcpy(
                    m_cameraResource.writeLocation,
                    &m_cameraResource.data,
                    m_cameraResource.allocationSize
                );
            }

            void Instance::updateCameraData(CameraComponent* inCamera)
            {
                if (!inCamera)
                {
                    destroyMeshData();

                    return;
                }

                if (isCameraDirty())
                {
                    setupCameraData(inCamera);

                    m_bIsCameraDirty = false;

                    return;
                }

                m_cameraResource.data = inCamera->getData();

                memcpy(
                    m_cameraResource.writeLocation,
                    &m_cameraResource.data,
                    m_cameraResource.allocationSize
                );
            }

            void Instance::destroyCameraData()
            {
                if (!m_cameraResource.buffer.memory)
                { 
                    return;
                }

                logicalDevice.unmapMemory(m_cameraResource.buffer.memory);

                Buffer::destroy(logicalDevice, m_cameraResource.buffer);
            }

            bool Instance::areMeshesDirty()
            {
                return m_bAreMeshesDirty;
            }

            void Instance::setMeshesAsDirty()
            {
                m_bAreMeshesDirty = true;
            }

            void Instance::setupMeshData(const std::vector<MeshComponent*>& inMeshes)
            {
                destroyMeshData();

                if (inMeshes.empty())
                {
                    return;
                }

                Buffer::CreateInfo bufferCreateInfo {};
                bufferCreateInfo.logicalDevice    = logicalDevice;
                bufferCreateInfo.physicalDevice   = physicalDevice;
                bufferCreateInfo.memoryProperties = vk::MemoryPropertyFlagBits::eHostVisible |
                                                    vk::MemoryPropertyFlagBits::eHostCoherent;
                bufferCreateInfo.size             = sizeof(Chicane::Mesh::Data) * inMeshes.size();
                bufferCreateInfo.usage            = vk::BufferUsageFlagBits::eStorageBuffer;

                Buffer::init(m_meshResource.buffer, bufferCreateInfo);
            
                m_meshResource.allocationSize = bufferCreateInfo.size;
                m_meshResource.writeLocation  = logicalDevice.mapMemory(
                    m_meshResource.buffer.memory,
                    0,
                    m_meshResource.allocationSize
                );

                modelDescriptorBufferInfo.buffer = m_meshResource.buffer.instance;
                modelDescriptorBufferInfo.offset = 0;
                modelDescriptorBufferInfo.range  = m_meshResource.allocationSize;

                m_meshResource.data.clear();
                m_meshResource.data.reserve(inMeshes.size());

                Texture::Manager* textureManager = Loader::getTextureManager();

                for (const MeshComponent* mesh : inMeshes)
                {
                    Chicane::Mesh::Data data {};
                    data.transform    = mesh->getPosition();
                    data.textureIndex = Vec<4, float>(textureManager->getIndex(mesh->getTexture()));

                    m_meshResource.data.push_back(data);
                }

                memcpy(
                    m_meshResource.writeLocation,
                    m_meshResource.data.data(),
                    m_meshResource.allocationSize
                );
            }

            void Instance::updateMeshData(const std::vector<MeshComponent*>& inMeshes)
            {
                if (inMeshes.empty())
                {
                    destroyMeshData();

                    return;
                }

                if (areMeshesDirty())
                {
                    setupMeshData(inMeshes);

                    m_bAreMeshesDirty = false;

                    return;
                }

                for (std::uint32_t i = 0; i < inMeshes.size(); i++)
                {
                    m_meshResource.data.at(i).transform = inMeshes.at(i)->getPosition();
                }

                memcpy(
                    m_meshResource.writeLocation,
                    m_meshResource.data.data(),
                    m_meshResource.allocationSize
                );
            }
        
            void Instance::destroyMeshData()
            {
                if (!m_meshResource.buffer.memory)
                {
                    return;
                }

                logicalDevice.unmapMemory(m_meshResource.buffer.memory);
    
                Buffer::destroy(logicalDevice, m_meshResource.buffer);
            }

            void Instance::setupDepthBuffer(const vk::Format& inFormat)
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

            void Instance::destroyDepthBuffer()
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

                destroyDepthBuffer();
                destroySync();
                destroyCameraData();
                destroyMeshData();

                logicalDevice.destroyImageView(imageView);
            }
        }
    }
}