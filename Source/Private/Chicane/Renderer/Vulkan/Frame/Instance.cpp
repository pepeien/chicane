#include "Chicane/Renderer/Vulkan/Frame/Instance.hpp"

#include "Chicane/Game/Level/Instance.hpp"
#include "Chicane/Game/Transformable/Component/Camera.hpp"
#include "Chicane/Game/Transformable/Component/Mesh.hpp"
#include "Chicane/Game/Transformable/Component/Light.hpp"
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

            void Instance::setupCameraData(const std::vector<CCamera*>& inCameras)
            {
                destroyCameraData();

                if (inCameras.empty())
                {
                    return;
                }

                Buffer::CreateInfo bufferCreateInfo = {};
                bufferCreateInfo.logicalDevice    = logicalDevice;
                bufferCreateInfo.physicalDevice   = physicalDevice;
                bufferCreateInfo.memoryProperties = vk::MemoryPropertyFlagBits::eHostVisible |
                                                    vk::MemoryPropertyFlagBits::eHostCoherent;
                bufferCreateInfo.size             = sizeof(Chicane::View::Data);
                bufferCreateInfo.usage            = vk::BufferUsageFlagBits::eUniformBuffer;

                cameraResource.setup(bufferCreateInfo);
                cameraResource.copyToBuffer(&inCameras.at(0)->getData());
            }

            void Instance::updateCameraData(const std::vector<CCamera*>& inCameras)
            {
                if (inCameras.empty())
                {
                    destroyCameraData();

                    return;
                }

                if (cameraResource.isDirty())
                {
                    setupCameraData(inCameras);

                    return;
                }

                cameraResource.copyToBuffer(&inCameras.at(0)->getData());
            }

            void Instance::destroyCameraData()
            {
                cameraResource.destroy(logicalDevice);
            }

            void Instance::setupLightData(const std::vector<CLight*>& inLights)
            {
                destroyLightData();

                if (inLights.empty())
                {
                    return;
                }

                Buffer::CreateInfo bufferCreateInfo = {};
                bufferCreateInfo.logicalDevice    = logicalDevice;
                bufferCreateInfo.physicalDevice   = physicalDevice;
                bufferCreateInfo.memoryProperties = vk::MemoryPropertyFlagBits::eHostVisible |
                                                    vk::MemoryPropertyFlagBits::eHostCoherent;
                bufferCreateInfo.size             = sizeof(Chicane::View::Data);
                bufferCreateInfo.usage            = vk::BufferUsageFlagBits::eUniformBuffer;

                lightResource.setup(bufferCreateInfo);
                lightResource.copyToBuffer(&inLights.at(0)->getData());
            }

            void Instance::updateLightData(const std::vector<CLight*>& inLights)
            {
                if (inLights.empty())
                {
                    destroyLightData();

                    return;
                }

                if (lightResource.isDirty())
                {
                    setupLightData(inLights);

                    return;
                }

                lightResource.copyToBuffer(&inLights.at(0)->getData());
            }

            void Instance::destroyLightData()
            {
                lightResource.destroy(logicalDevice);
            }

            void Instance::setupMeshData(const std::vector<CMesh*>& inMeshes)
            {
                destroyMeshData();

                if (inMeshes.empty())
                {
                    return;
                }

                Buffer::CreateInfo bufferCreateInfo = {};
                bufferCreateInfo.logicalDevice    = logicalDevice;
                bufferCreateInfo.physicalDevice   = physicalDevice;
                bufferCreateInfo.memoryProperties = vk::MemoryPropertyFlagBits::eHostVisible |
                                                    vk::MemoryPropertyFlagBits::eHostCoherent;
                bufferCreateInfo.size             = sizeof(Box::Mesh::CompiledData) * inMeshes.size();
                bufferCreateInfo.usage            = vk::BufferUsageFlagBits::eStorageBuffer;

                meshResource.setup(bufferCreateInfo);

                refreshMeshData(inMeshes);
            }

            void Instance::updateMeshData(const std::vector<CMesh*>& inMeshes)
            {
                std::vector<CMesh*> meshes = {};

                for (CMesh* mesh : inMeshes)
                {
                    if (!mesh->isDrawable())
                    {
                        continue;
                    }

                    meshes.push_back(mesh);
                }

                if (meshes.empty())
                {
                    destroyMeshData();

                    return;
                }

                if (meshResource.isDirty())
                {
                    setupMeshData(meshes);

                    return;
                }

                refreshMeshData(meshes);
            }
        
            void Instance::destroyMeshData()
            {
                meshResource.destroy(logicalDevice);
            }

            void Instance::setupColorImage(vk::Format inFormat)
            {
                colorImage.format = inFormat;

                Image::View::CreateInfo viewCreateInfo = {};
                viewCreateInfo.count         = 1;
                viewCreateInfo.type          = vk::ImageViewType::e2D;
                viewCreateInfo.aspect        = vk::ImageAspectFlagBits::eColor;
                viewCreateInfo.format        = inFormat;
                viewCreateInfo.logicalDevice = logicalDevice;
                Image::initView(colorImage.view, colorImage.instance, viewCreateInfo);
            }

            void Instance::destroyColorImage()
            {
                logicalDevice.destroyImageView(colorImage.view);
            }

            void Instance::setupDepthImage(vk::Format inFormat)
            {
                depthImage.format = inFormat;

                Image::Instance::CreateInfo instanceCreateInfo = {};
                instanceCreateInfo.flags         = vk::ImageCreateFlagBits();
                instanceCreateInfo.width         = width;
                instanceCreateInfo.height        = height;
                instanceCreateInfo.count         = 1;
                instanceCreateInfo.tiling        = vk::ImageTiling::eOptimal;
                instanceCreateInfo.usage         = vk::ImageUsageFlagBits::eDepthStencilAttachment | vk::ImageUsageFlagBits::eSampled;
                instanceCreateInfo.format        = inFormat;
                instanceCreateInfo.logicalDevice = logicalDevice;
                Image::initInstance(depthImage.instance, instanceCreateInfo);

                vk::SamplerCreateInfo createInfo = {};
                createInfo.flags                   = vk::SamplerCreateFlags();
                createInfo.minFilter               = vk::Filter::eLinear;
                createInfo.magFilter               = vk::Filter::eLinear;
                createInfo.mipmapMode              = vk::SamplerMipmapMode::eLinear;
                createInfo.mipLodBias              = 0.0f;
                createInfo.addressModeU            = vk::SamplerAddressMode::eClampToEdge;
                createInfo.addressModeV            = vk::SamplerAddressMode::eClampToEdge;
                createInfo.addressModeW            = vk::SamplerAddressMode::eClampToEdge;
                createInfo.anisotropyEnable        = false;
                createInfo.maxAnisotropy           = 1.0f;
                createInfo.borderColor             = vk::BorderColor::eFloatOpaqueWhite;
                createInfo.unnormalizedCoordinates = false;
                createInfo.compareEnable           = true;
                createInfo.compareOp               = vk::CompareOp::eLess;
                createInfo.minLod                  = 0.0f;
                createInfo.maxLod                  = 1.0f;
                createInfo.unnormalizedCoordinates = false;

                depthImage.sampler = logicalDevice.createSampler(createInfo);

                Image::Memory::CreateInfo memoryCreateInfo = {};
                memoryCreateInfo.properties     = vk::MemoryPropertyFlagBits::eDeviceLocal;
                memoryCreateInfo.logicalDevice  = logicalDevice;
                memoryCreateInfo.physicalDevice = physicalDevice;
                Image::initMemory(depthImage.memory, depthImage.instance, memoryCreateInfo);

                Image::View::CreateInfo viewCreateInfo = {};
                viewCreateInfo.count         = instanceCreateInfo.count;
                viewCreateInfo.type          = vk::ImageViewType::e2D;
                viewCreateInfo.aspect        = vk::ImageAspectFlagBits::eDepth;
                viewCreateInfo.format        = instanceCreateInfo.format;
                viewCreateInfo.logicalDevice = logicalDevice;
                Image::initView(depthImage.view, depthImage.instance, viewCreateInfo);
            }

            void Instance::destroyDepthImage()
            {
                logicalDevice.freeMemory(depthImage.memory);
                logicalDevice.destroyImage(depthImage.instance);
                logicalDevice.destroyImageView(depthImage.view);
                logicalDevice.destroySampler(depthImage.sampler);
            }

            void Instance::setupShadowImage(vk::Format inFormat)
            {
                shadowImage.format = inFormat;

                Image::Instance::CreateInfo instanceCreateInfo = {};
                instanceCreateInfo.width         = width;
                instanceCreateInfo.height        = height;
                instanceCreateInfo.count         = 1;
                instanceCreateInfo.tiling        = vk::ImageTiling::eOptimal;
                instanceCreateInfo.flags         = vk::ImageCreateFlagBits();
                instanceCreateInfo.usage         = vk::ImageUsageFlagBits::eDepthStencilAttachment | vk::ImageUsageFlagBits::eSampled;
                instanceCreateInfo.format        = inFormat;
                instanceCreateInfo.logicalDevice = logicalDevice;
                Image::initInstance(shadowImage.instance, instanceCreateInfo);

                vk::SamplerCreateInfo createInfo = {};
                createInfo.flags                   = vk::SamplerCreateFlags();
                createInfo.minFilter               = vk::Filter::eLinear;
                createInfo.magFilter               = vk::Filter::eLinear;
                createInfo.mipmapMode              = vk::SamplerMipmapMode::eLinear;
                createInfo.mipLodBias              = 0.0f;
                createInfo.addressModeU            = vk::SamplerAddressMode::eClampToEdge;
                createInfo.addressModeV            = vk::SamplerAddressMode::eClampToEdge;
                createInfo.addressModeW            = vk::SamplerAddressMode::eClampToEdge;
                createInfo.anisotropyEnable        = false;
                createInfo.maxAnisotropy           = 1.0f;
                createInfo.borderColor             = vk::BorderColor::eFloatOpaqueWhite;
                createInfo.unnormalizedCoordinates = false;
                createInfo.compareEnable           = true;
                createInfo.compareOp               = vk::CompareOp::eLessOrEqual;
                createInfo.minLod                  = 0.0f;
                createInfo.maxLod                  = 1.0f;
                createInfo.unnormalizedCoordinates = false;
                shadowImage.sampler = logicalDevice.createSampler(createInfo);

                Image::Memory::CreateInfo memoryCreateInfo = {};
                memoryCreateInfo.properties     = vk::MemoryPropertyFlagBits::eDeviceLocal;
                memoryCreateInfo.logicalDevice  = logicalDevice;
                memoryCreateInfo.physicalDevice = physicalDevice;
                Image::initMemory(shadowImage.memory, shadowImage.instance, memoryCreateInfo);

                Image::View::CreateInfo viewCreateInfo = {};
                viewCreateInfo.count         = instanceCreateInfo.count;
                viewCreateInfo.type          = vk::ImageViewType::e2D;
                viewCreateInfo.aspect        = vk::ImageAspectFlagBits::eDepth;
                viewCreateInfo.format        = instanceCreateInfo.format;
                viewCreateInfo.logicalDevice = logicalDevice;
                Image::initView(shadowImage.view, shadowImage.instance, viewCreateInfo);

                shadowImageInfo.imageLayout = vk::ImageLayout::eDepthAttachmentStencilReadOnlyOptimal;
                shadowImageInfo.imageView   = shadowImage.view;
                shadowImageInfo.sampler     = shadowImage.sampler;
            }

            void Instance::destroyShadowImage()
            {
                logicalDevice.freeMemory(shadowImage.memory);
                logicalDevice.destroyImage(shadowImage.instance);
                logicalDevice.destroyImageView(shadowImage.view);
                logicalDevice.destroySampler(shadowImage.sampler);
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

                destroyColorImage();
                destroyDepthImage();
                destroyShadowImage();
                destroySync();
                destroyCameraData();
                destroyLightData();
                destroyMeshData();
            }

            void Instance::refreshMeshData(const std::vector<CMesh*>& inMeshes)
            {
                if (inMeshes.empty())
                {
                    return;
                }

                Box::Texture::Manager* textureManager = Box::getTextureManager();

                std::vector<Box::Mesh::CompiledData> meshes = {};
                meshes.reserve(inMeshes.size());

                for (const CMesh* mesh : inMeshes)
                {
                    Box::Mesh::CompiledData data = {};
                    data.matrix       = mesh->getTransform().getMatrix();
                    data.textureIndex = Vec<4, float>(textureManager->getIndex(mesh->getTexture()));

                    meshes.push_back(data);
                }

                meshResource.copyToBuffer(meshes.data());
            }
        }
    }
}