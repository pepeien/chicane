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

            void Instance::setupCameraData(CameraComponent* inCamera)
            {
                Buffer::CreateInfo bufferCreateInfo {};
                bufferCreateInfo.logicalDevice    = logicalDevice;
                bufferCreateInfo.physicalDevice   = physicalDevice;
                bufferCreateInfo.memoryProperties = vk::MemoryPropertyFlagBits::eHostVisible |
                                                    vk::MemoryPropertyFlagBits::eHostCoherent;
                bufferCreateInfo.size             = sizeof(Chicane::Camera::Data);
                bufferCreateInfo.usage            = vk::BufferUsageFlagBits::eUniformBuffer;

                cameraResource.setup(bufferCreateInfo);
                cameraResource.copyToBuffer(&inCamera->getData());
            }

            void Instance::updateCameraData(CameraComponent* inCamera)
            {
                if (!inCamera)
                {
                    destroyMeshData();

                    return;
                }

                if (cameraResource.isDirty())
                {
                    setupCameraData(inCamera);

                    return;
                }

                cameraResource.copyToBuffer(&inCamera->getData());
            }

            void Instance::destroyCameraData()
            {
                cameraResource.destroy(logicalDevice);
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

                meshResource.setup(bufferCreateInfo);

                refreshMeshData(inMeshes);
            }

            void Instance::updateMeshData(const std::vector<MeshComponent*>& inMeshes)
            {
                if (inMeshes.empty())
                {
                    destroyMeshData();

                    return;
                }

                if (meshResource.isDirty())
                {
                    setupMeshData(inMeshes);

                    return;
                }

                refreshMeshData(inMeshes);
            }
        
            void Instance::destroyMeshData()
            {
                meshResource.destroy(logicalDevice);
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
                Image::initMemory(depth.memory, imageCreateInfo, depth.image);
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

            void Instance::refreshMeshData(const std::vector<MeshComponent*>& inMeshes)
            {
                if (inMeshes.empty())
                {
                    return;
                }

                Texture::Manager* textureManager = Loader::getTextureManager();

                std::vector<Chicane::Mesh::Data> meshes = {};
                meshes.reserve(inMeshes.size());

                for (const MeshComponent* mesh : inMeshes)
                {
                    Chicane::Mesh::Data data {};
                    data.transform    = mesh->getPosition();
                    data.textureIndex = Vec<4, float>(textureManager->getIndex(mesh->getTexture()));

                    meshes.push_back(data);
                }

                meshResource.copyToBuffer(meshes.data());
            }
        }
    }
}