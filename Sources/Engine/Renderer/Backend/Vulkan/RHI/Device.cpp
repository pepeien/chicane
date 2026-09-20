#include "Backend/Vulkan/RHI/Device.hpp"

#include <algorithm>
#include <cstdint>
#include <cstring>

#include "Chicane/Core/Math/Vertex.hpp"

#include "Backend/Vulkan.hpp"
#include "Backend/Vulkan/Allocator.hpp"
#include "Backend/Vulkan/Buffer/CreateInfo.hpp"
#include "Backend/Vulkan/CommandBuffer/Worker.hpp"
#include "Backend/Vulkan/Descriptor/Pool.hpp"
#include "Backend/Vulkan/Descriptor/Pool/CreateInfo.hpp"
#include "Backend/Vulkan/Descriptor/SetLayout.hpp"
#include "Backend/Vulkan/GraphicsPipeline/Builder.hpp"
#include "Backend/Vulkan/Image.hpp"
#include "Backend/Vulkan/Image/CreateInfo.hpp"
#include "Backend/Vulkan/Image/Memory/CreateInfo.hpp"
#include "Backend/Vulkan/Image/Sampler/CreateInfo.hpp"
#include "Backend/Vulkan/Image/View/CreateInfo.hpp"
#include "Backend/Vulkan/RHI/CommandList.hpp"
#include "Backend/Vulkan/Vertex.hpp"

namespace Chicane
{
    namespace Renderer
    {
        VulkanRHIDevice::VulkanRHIDevice(VulkanBackend* inBackend)
            : m_backend(inBackend),
              m_commands(std::make_unique<VulkanRHICommandList>(this))
        {}

        VulkanRHIDevice::~VulkanRHIDevice() = default;

        vk::Format VulkanRHIDevice::toVkFormat(RHI::ImageFormat inFormat) const
        {
            switch (inFormat)
            {
            case RHI::ImageFormat::RGBA8:
                return vk::Format::eR8G8B8A8Unorm;
            case RHI::ImageFormat::BGRA8:
                return vk::Format::eB8G8R8A8Unorm;
            case RHI::ImageFormat::RGBA16F:
                return vk::Format::eR16G16B16A16Sfloat;
            case RHI::ImageFormat::Depth32F:
                return vk::Format::eD32Sfloat;
            case RHI::ImageFormat::Depth24Stencil8:
                return vk::Format::eD24UnormS8Uint;
            default:
                return vk::Format::eR8G8B8A8Unorm;
            }
        }

        vk::ImageLayout VulkanRHIDevice::shaderLayout(RHI::ImageKind inKind) const
        {
            if (inKind == RHI::ImageKind::Depth2D || inKind == RHI::ImageKind::Depth2DArray)
            {
                return vk::ImageLayout::eDepthStencilReadOnlyOptimal;
            }

            return vk::ImageLayout::eShaderReadOnlyOptimal;
        }

        std::uint64_t VulkanRHIDevice::imageKey(const VulkanRHIImageData* inImage) const
        {
            return static_cast<std::uint64_t>(
                reinterpret_cast<std::uintptr_t>(static_cast<VkImage>(inImage->info.instance))
            );
        }

        vk::ImageLayout VulkanRHIDevice::rememberedLayout(const VulkanRHIImageData* inImage) const
        {
            if (!inImage)
            {
                return vk::ImageLayout::eUndefined;
            }

            const auto found = m_imageLayouts.find(imageKey(inImage));
            if (found != m_imageLayouts.end())
            {
                return found->second;
            }

            return inImage->layout;
        }

        void VulkanRHIDevice::rememberLayout(VulkanRHIImageData* inImage, vk::ImageLayout inLayout)
        {
            if (!inImage)
            {
                return;
            }

            inImage->layout                   = inLayout;
            m_imageLayouts[imageKey(inImage)] = inLayout;
        }

        RHI::Buffer VulkanRHIDevice::createBuffer(const RHI::BufferCreateInfo& inCreateInfo)
        {
            auto* data   = new VulkanRHIBufferData();
            data->size   = inCreateInfo.size;
            data->bHost  = inCreateInfo.bHasHostAccess;
            data->bOwned = true;

            VulkanBufferCreateInfo info;
            info.logicalDevice  = m_backend->logicalDevice;
            info.physicalDevice = m_backend->physicalDevice;
            info.allocator      = &m_backend->allocator;
            info.size           = inCreateInfo.size;
            info.usage          = vk::BufferUsageFlagBits::eTransferDst | vk::BufferUsageFlagBits::eTransferSrc;
            if (inCreateInfo.usage == RHI::BufferUsage::Vertex)
            {
                info.usage |= vk::BufferUsageFlagBits::eVertexBuffer;
            }
            else if (inCreateInfo.usage == RHI::BufferUsage::Index)
            {
                info.usage |= vk::BufferUsageFlagBits::eIndexBuffer;
            }
            else if (inCreateInfo.usage == RHI::BufferUsage::Uniform)
            {
                info.usage |= vk::BufferUsageFlagBits::eUniformBuffer;
            }
            else
            {
                info.usage |= vk::BufferUsageFlagBits::eStorageBuffer;
            }

            if (inCreateInfo.bHasHostAccess)
            {
                info.memoryProperties =
                    vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent;
            }
            else
            {
                info.memoryProperties = vk::MemoryPropertyFlagBits::eDeviceLocal;
            }

            data->buffer.init(info);
            data->usage = info.usage;
            if (inCreateInfo.bHasHostAccess)
            {
                data->mapped = data->buffer.map();
            }

            return {data};
        }

        void VulkanRHIDevice::destroyBuffer(RHI::Buffer inBuffer)
        {
            auto* data = static_cast<VulkanRHIBufferData*>(inBuffer.handle);
            if (!data)
            {
                return;
            }

            if (data->bOwned)
            {
                data->mapped = nullptr;
                data->buffer.destroy();
            }
            delete data;
        }

        void VulkanRHIDevice::updateBuffer(
            RHI::Buffer inBuffer, const void* inData, std::size_t inSize, std::size_t inOffset
        )
        {
            auto* data = static_cast<VulkanRHIBufferData*>(inBuffer.handle);
            if (!data || !inData || inSize == 0)
            {
                return;
            }

            if (inOffset + inSize > data->size)
            {
                VulkanBufferCreateInfo info;
                info.logicalDevice    = m_backend->logicalDevice;
                info.physicalDevice   = m_backend->physicalDevice;
                info.allocator        = &m_backend->allocator;
                info.size             = std::max(inOffset + inSize, data->size * 2);
                info.usage            = data->usage;
                info.memoryProperties = data->bHost
                                            ? vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent
                                            : vk::MemoryPropertyFlagBits::eDeviceLocal;

                VulkanBuffer grown;
                grown.init(info);
                if (data->bHost && data->mapped && data->size > 0)
                {
                    std::memcpy(grown.map(), data->mapped, static_cast<std::size_t>(data->size));
                }
                else if (!data->bHost && data->size > 0)
                {
                    data->buffer.copy(grown, data->size, m_backend->graphicsQueue, m_backend->mainCommandBuffer);
                }

                data->buffer.destroy();
                data->buffer = grown;
                data->size   = info.size;
                data->mapped = data->bHost ? data->buffer.map() : nullptr;
            }

            if (data->mapped)
            {
                std::memcpy(static_cast<char*>(data->mapped) + inOffset, inData, inSize);
                return;
            }

            m_backend->allocator.upload(
                data->buffer.instance,
                inOffset,
                inData,
                inSize,
                m_backend->mainCommandBuffer,
                m_backend->graphicsQueue
            );
        }

        RHI::Image VulkanRHIDevice::createImage(const RHI::ImageCreateInfo& inCreateInfo)
        {
            auto* data        = new VulkanRHIImageData();
            data->kind        = inCreateInfo.kind;
            data->format      = inCreateInfo.format;
            data->layers      = inCreateInfo.layers;
            data->mips        = inCreateInfo.mipLevels;
            data->bOwned      = true;
            data->info.format = toVkFormat(inCreateInfo.format);
            data->info.extent = vk::Extent2D{inCreateInfo.width, inCreateInfo.height};

            VulkanImageCreateInfo instanceCreateInfo;
            instanceCreateInfo.width     = inCreateInfo.width;
            instanceCreateInfo.height    = inCreateInfo.height;
            instanceCreateInfo.count     = inCreateInfo.kind == RHI::ImageKind::Cube ? 6 : inCreateInfo.layers;
            instanceCreateInfo.mipLevels = inCreateInfo.mipLevels;
            instanceCreateInfo.tiling    = vk::ImageTiling::eOptimal;
            instanceCreateInfo.flags     = inCreateInfo.kind == RHI::ImageKind::Cube
                                               ? vk::ImageCreateFlagBits::eCubeCompatible
                                               : vk::ImageCreateFlags();
            instanceCreateInfo.usage     = vk::ImageUsageFlagBits::eTransferDst | vk::ImageUsageFlagBits::eTransferSrc;
            if (inCreateInfo.bIsSampled)
            {
                instanceCreateInfo.usage |= vk::ImageUsageFlagBits::eSampled;
            }
            if (inCreateInfo.bHasColor)
            {
                instanceCreateInfo.usage |= vk::ImageUsageFlagBits::eColorAttachment;
            }
            if (inCreateInfo.bHasDepth)
            {
                instanceCreateInfo.usage |= vk::ImageUsageFlagBits::eDepthStencilAttachment;
            }
            instanceCreateInfo.format        = data->info.format;
            instanceCreateInfo.logicalDevice = m_backend->logicalDevice;

            VulkanImageMemoryCreateInfo memoryCreateInfo;
            memoryCreateInfo.properties     = vk::MemoryPropertyFlagBits::eDeviceLocal;
            memoryCreateInfo.logicalDevice  = m_backend->logicalDevice;
            memoryCreateInfo.physicalDevice = m_backend->physicalDevice;
            memoryCreateInfo.allocator      = &m_backend->allocator;
            VulkanImage::init(data->info, instanceCreateInfo, memoryCreateInfo);

            VulkanImageViewCreateInfo viewCreateInfo;
            viewCreateInfo.count         = instanceCreateInfo.count;
            viewCreateInfo.mipLevels     = inCreateInfo.mipLevels;
            viewCreateInfo.format        = data->info.format;
            viewCreateInfo.logicalDevice = m_backend->logicalDevice;
            if (inCreateInfo.kind == RHI::ImageKind::Cube)
            {
                viewCreateInfo.type   = vk::ImageViewType::eCube;
                viewCreateInfo.aspect = vk::ImageAspectFlagBits::eColor;
            }
            else if (inCreateInfo.kind == RHI::ImageKind::Depth2DArray)
            {
                viewCreateInfo.type   = vk::ImageViewType::e2DArray;
                viewCreateInfo.aspect = vk::ImageAspectFlagBits::eDepth;
            }
            else if (inCreateInfo.kind == RHI::ImageKind::Depth2D)
            {
                viewCreateInfo.type   = vk::ImageViewType::e2D;
                viewCreateInfo.aspect = VulkanImage::depthAspect(data->info.format);
            }
            else
            {
                viewCreateInfo.type   = vk::ImageViewType::e2D;
                viewCreateInfo.aspect = vk::ImageAspectFlagBits::eColor;
            }
            VulkanImage::initView(data->info.view, data->info.instance, viewCreateInfo);

            data->bIsSampled = inCreateInfo.bIsSampled;
            data->bHasColor  = inCreateInfo.bHasColor;
            data->bHasDepth  = inCreateInfo.bHasDepth;
            const vk::ImageLayout finalLayout =
                inCreateInfo.bHasDepth ? (inCreateInfo.bIsSampled ? vk::ImageLayout::eDepthStencilReadOnlyOptimal
                                                                  : vk::ImageLayout::eDepthStencilAttachmentOptimal)
                                       : (inCreateInfo.bIsSampled ? vk::ImageLayout::eShaderReadOnlyOptimal
                                                                  : vk::ImageLayout::eColorAttachmentOptimal);
            VulkanImage::transitionLayout(
                m_backend->mainCommandBuffer,
                m_backend->graphicsQueue,
                data->info.instance,
                vk::ImageLayout::eUndefined,
                finalLayout,
                instanceCreateInfo.count,
                inCreateInfo.mipLevels,
                viewCreateInfo.aspect
            );
            rememberLayout(data, finalLayout);

            return {data};
        }

        void VulkanRHIDevice::destroyImage(RHI::Image inImage)
        {
            auto* data = static_cast<VulkanRHIImageData*>(inImage.handle);
            if (!data)
            {
                return;
            }

            if (data->bOwned)
            {
                if (!data->bViewOnly)
                {
                    m_imageLayouts.erase(imageKey(data));
                }
                if (data->info.view)
                {
                    m_backend->logicalDevice.destroyImageView(data->info.view);
                }
                if (!data->bViewOnly)
                {
                    VulkanAllocator::destroyImage(data->info);
                    if (data->info.sampler)
                    {
                        m_backend->logicalDevice.destroySampler(data->info.sampler);
                    }
                }
            }
            delete data;
        }

        void VulkanRHIDevice::updateImage(
            RHI::Image inImage, const void* inData, std::uint32_t inWidth, std::uint32_t inHeight, std::uint32_t inLayer
        )
        {
            auto* data = static_cast<VulkanRHIImageData*>(inImage.handle);
            if (!data || !inData)
            {
                return;
            }

            const std::size_t size =
                static_cast<std::size_t>(inWidth) * inHeight * (data->format == RHI::ImageFormat::RGBA16F ? 8u : 4u);

            VulkanBufferCreateInfo info;
            info.logicalDevice    = m_backend->logicalDevice;
            info.physicalDevice   = m_backend->physicalDevice;
            info.allocator        = &m_backend->allocator;
            info.size             = size;
            info.usage            = vk::BufferUsageFlagBits::eTransferSrc;
            info.memoryProperties =
                vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent;
            VulkanBuffer staging;
            staging.init(info);
            std::memcpy(staging.map(), inData, size);

            if (inLayer == 0)
            {
                VulkanImage::transitionLayout(
                    m_backend->mainCommandBuffer,
                    m_backend->graphicsQueue,
                    data->info.instance,
                    rememberedLayout(data),
                    vk::ImageLayout::eTransferDstOptimal,
                    data->kind == RHI::ImageKind::Cube ? 6 : data->layers,
                    data->mips
                );
                rememberLayout(data, vk::ImageLayout::eTransferDstOptimal);
            }

            VulkanCommandBufferWorker::startJob(m_backend->mainCommandBuffer);
            vk::ImageSubresourceLayers layers;
            layers.aspectMask     = vk::ImageAspectFlagBits::eColor;
            layers.mipLevel       = 0;
            layers.baseArrayLayer = inLayer;
            layers.layerCount     = 1;
            vk::BufferImageCopy copy;
            copy.imageSubresource = layers;
            copy.imageExtent      = vk::Extent3D(inWidth, inHeight, 1);
            m_backend->mainCommandBuffer
                .copyBufferToImage(staging.instance, data->info.instance, vk::ImageLayout::eTransferDstOptimal, copy);
            VulkanCommandBufferWorker::endJob(m_backend->mainCommandBuffer, m_backend->graphicsQueue, "RHI image");
            staging.destroy();
        }

        void VulkanRHIDevice::generateMips(RHI::Image inImage)
        {
            auto* data = static_cast<VulkanRHIImageData*>(inImage.handle);
            if (!data)
            {
                return;
            }

            VulkanImage::generateMipmaps(
                m_backend->mainCommandBuffer,
                m_backend->graphicsQueue,
                data->info.instance,
                data->info.extent.width,
                data->info.extent.height,
                data->kind == RHI::ImageKind::Cube ? 6 : data->layers,
                data->mips
            );
            rememberLayout(data, vk::ImageLayout::eShaderReadOnlyOptimal);
        }

        RHI::Image VulkanRHIDevice::createImageView(RHI::Image inImage, std::uint32_t inLayer)
        {
            auto* source     = static_cast<VulkanRHIImageData*>(inImage.handle);
            auto* data       = new VulkanRHIImageData();
            data->kind       = RHI::ImageKind::Depth2D;
            data->format     = source->format;
            data->layers     = 1;
            data->mips       = 1;
            data->bOwned     = true;
            data->bViewOnly  = true;
            data->bIsSampled = source->bIsSampled;
            data->bHasColor  = source->bHasColor;
            data->bHasDepth  = source->bHasDepth;
            data->layout     = source->layout;
            data->parent     = source;
            data->info       = source->info;
            data->info.view  = nullptr;

            VulkanImageViewCreateInfo viewCreateInfo;
            viewCreateInfo.count          = 1;
            viewCreateInfo.baseArrayLayer = inLayer;
            viewCreateInfo.type           = vk::ImageViewType::e2D;
            viewCreateInfo.aspect         = vk::ImageAspectFlagBits::eDepth;
            viewCreateInfo.format         = source->info.format;
            viewCreateInfo.logicalDevice  = m_backend->logicalDevice;
            VulkanImage::initView(data->info.view, source->info.instance, viewCreateInfo);

            return {data};
        }

        RHI::Sampler VulkanRHIDevice::createSampler(const RHI::SamplerCreateInfo& inCreateInfo)
        {
            auto*                 data = new VulkanRHISamplerData();
            vk::SamplerCreateInfo info;
            info.minFilter =
                inCreateInfo.minFilter == RHI::SamplerFilter::Nearest ? vk::Filter::eNearest : vk::Filter::eLinear;
            info.magFilter =
                inCreateInfo.magFilter == RHI::SamplerFilter::Nearest ? vk::Filter::eNearest : vk::Filter::eLinear;
            info.mipmapMode   = inCreateInfo.bHasMip ? vk::SamplerMipmapMode::eLinear : vk::SamplerMipmapMode::eNearest;
            info.addressModeU = vk::SamplerAddressMode::eClampToEdge;
            if (inCreateInfo.address == RHI::SamplerAddress::ClampToBorder)
            {
                info.addressModeU = vk::SamplerAddressMode::eClampToBorder;
            }
            else if (inCreateInfo.address == RHI::SamplerAddress::Repeat)
            {
                info.addressModeU = vk::SamplerAddressMode::eRepeat;
            }
            info.addressModeV = info.addressModeU;
            info.addressModeW = info.addressModeU;
            info.borderColor  = vk::BorderColor::eFloatOpaqueWhite;
            info.maxLod       = inCreateInfo.bHasMip ? 16.0f : 1.0f;
            data->sampler     = m_backend->logicalDevice.createSampler(info);
            return {data};
        }

        void VulkanRHIDevice::destroySampler(RHI::Sampler inSampler)
        {
            auto* data = static_cast<VulkanRHISamplerData*>(inSampler.handle);
            if (!data)
            {
                return;
            }
            if (data->bOwned && data->sampler)
            {
                m_backend->logicalDevice.destroySampler(data->sampler);
            }
            delete data;
        }

        RHI::BindGroupLayout VulkanRHIDevice::createBindGroupLayout(const RHI::BindGroupLayoutCreateInfo& inCreateInfo)
        {
            auto* data       = new VulkanRHILayoutData();
            data->createInfo = inCreateInfo;

            VulkanDescriptorSetLayoutBidingsCreateInfo bidings;
            bidings.count = static_cast<std::uint32_t>(inCreateInfo.bindings.size());
            for (const RHI::Binding& binding : inCreateInfo.bindings)
            {
                bidings.indices.push_back(binding.binding);
                vk::DescriptorType type = vk::DescriptorType::eUniformBuffer;
                if (binding.type == RHI::BindingType::StorageBuffer)
                {
                    type = vk::DescriptorType::eStorageBuffer;
                }
                else if (binding.type == RHI::BindingType::SampledImage)
                {
                    type = vk::DescriptorType::eCombinedImageSampler;
                }
                bidings.types.push_back(type);
                bidings.counts.push_back(binding.count);
                vk::ShaderStageFlags stages;
                if (binding.bIsVertex)
                {
                    stages |= vk::ShaderStageFlagBits::eVertex;
                }
                if (binding.bIsFragment)
                {
                    stages |= vk::ShaderStageFlagBits::eFragment;
                }
                bidings.stages.push_back(stages);
                if (binding.bIsPartiallyBound)
                {
                    bidings.bindingFlags.push_back(vk::DescriptorBindingFlagBits::ePartiallyBound);
                }
            }
            VulkanDescriptorSetLayout::init(data->layout, m_backend->logicalDevice, bidings);
            return {data};
        }

        void VulkanRHIDevice::destroyBindGroupLayout(RHI::BindGroupLayout inLayout)
        {
            auto* data = static_cast<VulkanRHILayoutData*>(inLayout.handle);
            if (!data)
            {
                return;
            }
            m_backend->logicalDevice.destroyDescriptorSetLayout(data->layout);
            delete data;
        }

        void VulkanRHIDevice::writeGroup(VulkanRHIGroupData* inGroup, const std::vector<RHI::BindResource>& inResources)
        {
            std::vector<vk::WriteDescriptorSet>   writes;
            std::vector<vk::DescriptorBufferInfo> buffers;
            std::vector<vk::DescriptorImageInfo>  images;
            buffers.reserve(inResources.size());
            images.reserve(inResources.size());
            writes.reserve(inResources.size());

            for (const RHI::BindResource& resource : inResources)
            {
                vk::WriteDescriptorSet write;
                write.dstSet          = inGroup->set;
                write.dstBinding      = resource.binding;
                write.dstArrayElement = resource.arrayIndex;
                write.descriptorCount = 1;

                if (resource.type == RHI::BindingType::SampledImage)
                {
                    auto*                   image   = static_cast<VulkanRHIImageData*>(resource.image.handle);
                    auto*                   sampler = static_cast<VulkanRHISamplerData*>(resource.sampler.handle);
                    vk::DescriptorImageInfo info;
                    info.imageLayout = shaderLayout(image->kind);
                    info.imageView   = image->info.view;
                    info.sampler     = sampler ? sampler->sampler : image->info.sampler;
                    images.push_back(info);
                    write.descriptorType = vk::DescriptorType::eCombinedImageSampler;
                    write.pImageInfo     = &images.back();
                }
                else
                {
                    auto*                    buffer = static_cast<VulkanRHIBufferData*>(resource.buffer.handle);
                    vk::DescriptorBufferInfo info;
                    info.buffer = buffer->buffer.instance;
                    info.offset = 0;
                    info.range  = buffer->size;
                    buffers.push_back(info);
                    write.descriptorType = resource.type == RHI::BindingType::StorageBuffer
                                               ? vk::DescriptorType::eStorageBuffer
                                               : vk::DescriptorType::eUniformBuffer;
                    write.pBufferInfo    = &buffers.back();
                }
                writes.push_back(write);
            }

            if (!writes.empty())
            {
                m_backend->logicalDevice.updateDescriptorSets(writes, nullptr);
            }
        }

        RHI::BindGroup VulkanRHIDevice::createBindGroup(
            RHI::BindGroupLayout inLayout, const std::vector<RHI::BindResource>& inResources
        )
        {
            auto* layout = static_cast<VulkanRHILayoutData*>(inLayout.handle);
            auto* data   = new VulkanRHIGroupData();
            data->layout = layout->layout;
            data->bOwned = true;

            VulkanDescriptorPoolCreateInfo poolInfo;
            poolInfo.maxSets = 1;
            poolInfo.sizes.push_back({vk::DescriptorType::eUniformBuffer, 8});
            poolInfo.sizes.push_back({vk::DescriptorType::eStorageBuffer, 8});
            poolInfo.sizes.push_back({vk::DescriptorType::eCombinedImageSampler, 16});
            VulkanDescriptorPool::init(data->pool, m_backend->logicalDevice, poolInfo);
            VulkanDescriptorSetLayout::allocate(data->set, m_backend->logicalDevice, data->layout, data->pool);
            writeGroup(data, inResources);
            return {data};
        }

        void VulkanRHIDevice::updateBindGroup(RHI::BindGroup inGroup, const std::vector<RHI::BindResource>& inResources)
        {
            auto* data = static_cast<VulkanRHIGroupData*>(inGroup.handle);
            if (!data)
            {
                return;
            }
            writeGroup(data, inResources);
        }

        void VulkanRHIDevice::destroyBindGroup(RHI::BindGroup inGroup)
        {
            auto* data = static_cast<VulkanRHIGroupData*>(inGroup.handle);
            if (!data)
            {
                return;
            }
            if (data->bOwned && data->pool)
            {
                m_backend->logicalDevice.destroyDescriptorPool(data->pool);
            }
            delete data;
        }

        static vk::Format toVertexFormat(RHI::VertexFormat inFormat)
        {
            switch (inFormat)
            {
            case RHI::VertexFormat::Float2:
                return vk::Format::eR32G32Sfloat;
            case RHI::VertexFormat::Float4:
                return vk::Format::eR32G32B32A32Sfloat;
            default:
                return vk::Format::eR32G32B32Sfloat;
            }
        }

        static vk::CompareOp toVkCompare(DepthCompare inValue)
        {
            switch (inValue)
            {
            case DepthCompare::Never:
                return vk::CompareOp::eNever;
            case DepthCompare::Equal:
                return vk::CompareOp::eEqual;
            case DepthCompare::LessOrEqual:
                return vk::CompareOp::eLessOrEqual;
            case DepthCompare::Greater:
                return vk::CompareOp::eGreater;
            case DepthCompare::NotEqual:
                return vk::CompareOp::eNotEqual;
            case DepthCompare::GreaterOrEqual:
                return vk::CompareOp::eGreaterOrEqual;
            case DepthCompare::Always:
                return vk::CompareOp::eAlways;
            default:
                return vk::CompareOp::eLess;
            }
        }

        RHI::Pipeline VulkanRHIDevice::createPipeline(const RHI::PipelineCreateInfo& inCreateInfo)
        {
            auto* data     = new VulkanRHIPipelineData();
            data->pushSize = inCreateInfo.pushConstantSize;
            if (inCreateInfo.bHasPushVertex)
            {
                data->pushStages |= vk::ShaderStageFlagBits::eVertex;
            }
            if (inCreateInfo.bHasPushFragment)
            {
                data->pushStages |= vk::ShaderStageFlagBits::eFragment;
            }

            VulkanShaderStageCreateInfo vertexShader;
            vertexShader.path = shaderPath(inCreateInfo.vertexPath, ShaderType::Vertex);
            vertexShader.type = vk::ShaderStageFlagBits::eVertex;

            vk::PipelineRasterizationStateCreateInfo rasterization;
            rasterization.polygonMode =
                inCreateInfo.fill == RHI::FillMode::Line ? vk::PolygonMode::eLine : vk::PolygonMode::eFill;
            rasterization.cullMode  = inCreateInfo.cull == CullingMode::Front  ? vk::CullModeFlagBits::eFront
                                      : inCreateInfo.cull == CullingMode::Back ? vk::CullModeFlagBits::eBack
                                      : inCreateInfo.cull == CullingMode::FrontAndBack
                                          ? vk::CullModeFlagBits::eFrontAndBack
                                          : vk::CullModeFlagBits::eNone;
            rasterization.frontFace = inCreateInfo.frontFace == CullingFrontFace::Clockwise
                                          ? vk::FrontFace::eClockwise
                                          : vk::FrontFace::eCounterClockwise;
            rasterization.lineWidth = 1.0f;
            if (inCreateInfo.fill == RHI::FillMode::Line ||
                inCreateInfo.topology == RHI::PrimitiveTopology::LineList)
            {
                rasterization.depthBiasEnable         = VK_TRUE;
                rasterization.depthBiasConstantFactor = -1.25f;
                rasterization.depthBiasSlopeFactor    = -1.0f;
            }

            vk::PipelineDepthStencilStateCreateInfo depth;
            depth.depthTestEnable  = inCreateInfo.bHasDepthTest;
            depth.depthWriteEnable = inCreateInfo.bHasDepthWrite;
            depth.depthCompareOp   = toVkCompare(inCreateInfo.depthCompare);
            if (inCreateInfo.stencil == RHI::StencilMode::WriteReplace)
            {
                vk::StencilOpState stencil;
                stencil.failOp          = vk::StencilOp::eKeep;
                stencil.passOp          = vk::StencilOp::eReplace;
                stencil.depthFailOp     = vk::StencilOp::eKeep;
                stencil.compareOp       = vk::CompareOp::eAlways;
                stencil.compareMask     = 0xFF;
                stencil.writeMask       = 0xFF;
                stencil.reference       = 1;
                depth.stencilTestEnable = true;
                depth.front             = stencil;
                depth.back              = stencil;
            }
            else if (inCreateInfo.stencil == RHI::StencilMode::TestNotEqual)
            {
                vk::StencilOpState stencil;
                stencil.failOp          = vk::StencilOp::eKeep;
                stencil.passOp          = vk::StencilOp::eKeep;
                stencil.depthFailOp     = vk::StencilOp::eKeep;
                stencil.compareOp       = vk::CompareOp::eNotEqual;
                stencil.compareMask     = 0xFF;
                stencil.writeMask       = 0x00;
                stencil.reference       = 1;
                depth.stencilTestEnable = true;
                depth.front             = stencil;
                depth.back              = stencil;
            }

            vk::PipelineColorBlendAttachmentState blend =
                inCreateInfo.blend == RHI::BlendMode::Additive
                    ? VulkanGraphicsPipeline::createAdditiveBlendAttachmentState()
                    : VulkanGraphicsPipeline::createBlendAttachmentState(inCreateInfo.blend == RHI::BlendMode::Alpha);
            if (!inCreateInfo.bHasColorWrite)
            {
                blend.colorWriteMask = {};
            }

            auto builder = VulkanGraphicsPipelineBuilder();
            if (inCreateInfo.vertexStride > 0)
            {
                vk::VertexInputBindingDescription binding;
                binding.binding   = 0;
                binding.stride    = inCreateInfo.vertexStride;
                binding.inputRate = vk::VertexInputRate::eVertex;
                builder           = builder.addVertexBinding(binding);
                for (const RHI::VertexAttribute& attribute : inCreateInfo.vertexAttributes)
                {
                    vk::VertexInputAttributeDescription desc;
                    desc.binding  = 0;
                    desc.location = attribute.location;
                    desc.format   = toVertexFormat(attribute.format);
                    desc.offset   = attribute.offset;
                    builder       = builder.addVertexAttribute(desc);
                }
            }

            vk::PipelineInputAssemblyStateCreateInfo assembly = VulkanGraphicsPipeline::createInputAssemblyState();
            if (inCreateInfo.topology == RHI::PrimitiveTopology::TriangleStrip)
            {
                assembly.topology = vk::PrimitiveTopology::eTriangleStrip;
            }
            else if (inCreateInfo.topology == RHI::PrimitiveTopology::LineList)
            {
                assembly.topology = vk::PrimitiveTopology::eLineList;
            }

            vk::Viewport viewport;
            viewport.width    = 1.0f;
            viewport.height   = 1.0f;
            viewport.minDepth = 0.0f;
            viewport.maxDepth = 1.0f;
            vk::Rect2D scissor;
            scissor.extent = vk::Extent2D{1, 1};

            builder = builder.setInputAssembly(assembly)
                          .addViewport(viewport)
                          .addDynamicState(vk::DynamicState::eViewport)
                          .addScissor(scissor)
                          .addDynamicState(vk::DynamicState::eScissor)
                          .addDynamicState(vk::DynamicState::eLineWidth)
                          .addShaderStage(vertexShader, m_backend->logicalDevice)
                          .setRasterization(rasterization)
                          .setDepthStencil(depth);

            if (!inCreateInfo.fragmentPath.isEmpty())
            {
                VulkanShaderStageCreateInfo fragmentShader;
                fragmentShader.path = shaderPath(inCreateInfo.fragmentPath, ShaderType::Fragment);
                fragmentShader.type = vk::ShaderStageFlagBits::eFragment;
                builder             = builder.addShaderStage(fragmentShader, m_backend->logicalDevice);
            }

            std::vector<vk::AttachmentDescription> attachments;
            std::vector<vk::AttachmentReference>   colorRefs;
            vk::AttachmentReference                depthRef;
            if (inCreateInfo.bHasColor)
            {
                vk::AttachmentDescription color;
                color.format        = toVkFormat(inCreateInfo.colorFormat);
                color.samples       = vk::SampleCountFlagBits::e1;
                color.loadOp        = vk::AttachmentLoadOp::eLoad;
                color.storeOp       = vk::AttachmentStoreOp::eStore;
                color.initialLayout = vk::ImageLayout::eColorAttachmentOptimal;
                color.finalLayout   = vk::ImageLayout::eColorAttachmentOptimal;
                builder             = builder.addAttachment(color);
                vk::AttachmentReference ref;
                ref.attachment = 0;
                ref.layout     = vk::ImageLayout::eColorAttachmentOptimal;
                colorRefs.push_back(ref);
                builder = builder.addColorBlendingAttachment(blend);
            }
            if (inCreateInfo.bHasDepth)
            {
                vk::AttachmentDescription depthAttachment;
                depthAttachment.format         = inCreateInfo.depthFormat == RHI::ImageFormat::Depth24Stencil8
                                                     ? m_backend->swapchain.depthFormat
                                                     : toVkFormat(inCreateInfo.depthFormat);
                depthAttachment.samples        = vk::SampleCountFlagBits::e1;
                depthAttachment.loadOp         = vk::AttachmentLoadOp::eLoad;
                depthAttachment.storeOp        = vk::AttachmentStoreOp::eStore;
                depthAttachment.stencilLoadOp  = vk::AttachmentLoadOp::eLoad;
                depthAttachment.stencilStoreOp = vk::AttachmentStoreOp::eStore;
                depthAttachment.initialLayout  = vk::ImageLayout::eDepthStencilAttachmentOptimal;
                depthAttachment.finalLayout    = vk::ImageLayout::eDepthStencilAttachmentOptimal;
                builder                        = builder.addAttachment(depthAttachment);
                depthRef.attachment            = inCreateInfo.bHasColor ? 1 : 0;
                depthRef.layout                = vk::ImageLayout::eDepthStencilAttachmentOptimal;
            }

            vk::SubpassDescription subpass;
            subpass.pipelineBindPoint = vk::PipelineBindPoint::eGraphics;
            if (!colorRefs.empty())
            {
                subpass.colorAttachmentCount = 1;
                subpass.pColorAttachments    = colorRefs.data();
            }
            if (inCreateInfo.bHasDepth)
            {
                subpass.pDepthStencilAttachment = &depthRef;
            }
            builder = builder.addSubpass(subpass);

            for (RHI::BindGroupLayout layout : inCreateInfo.layouts)
            {
                auto* layoutData = static_cast<VulkanRHILayoutData*>(layout.handle);
                builder          = builder.addDescriptorSetLayout(layoutData->layout);
            }

            if (inCreateInfo.pushConstantSize > 0)
            {
                vk::PushConstantRange range;
                range.offset = 0;
                range.size   = inCreateInfo.pushConstantSize;
                if (inCreateInfo.bHasPushVertex)
                {
                    range.stageFlags |= vk::ShaderStageFlagBits::eVertex;
                }
                if (inCreateInfo.bHasPushFragment)
                {
                    range.stageFlags |= vk::ShaderStageFlagBits::eFragment;
                }
                builder = builder.addPushConstant(range);
            }

            builder.build(data->pipeline, m_backend->logicalDevice);
            return {data};
        }

        void VulkanRHIDevice::destroyPipeline(RHI::Pipeline inPipeline)
        {
            auto* data = static_cast<VulkanRHIPipelineData*>(inPipeline.handle);
            if (!data)
            {
                return;
            }
            data->pipeline.destroy();
            delete data;
        }

        String VulkanRHIDevice::shaderPath(const String& inLogical, ShaderType inType) const
        {
            return inLogical + (inType == ShaderType::Vertex ? ".vert.vspv" : ".frag.vspv");
        }

        RHI::ImageFormat VulkanRHIDevice::sceneColorFormat() const
        {
            return RHI::ImageFormat::RGBA16F;
        }

        RHI::ImageFormat VulkanRHIDevice::sceneDepthFormat() const
        {
            return VulkanImage::hasStencil(m_backend->swapchain.depthFormat) ? RHI::ImageFormat::Depth24Stencil8
                                                                             : RHI::ImageFormat::Depth32F;
        }

        RHI::ImageFormat VulkanRHIDevice::presentColorFormat() const
        {
            return m_backend->swapchain.colorFormat == vk::Format::eR8G8B8A8Unorm ? RHI::ImageFormat::RGBA8
                                                                                  : RHI::ImageFormat::BGRA8;
        }

        RHI::Buffer VulkanRHIDevice::wrapBuffer(const VulkanBuffer& inBuffer, vk::DeviceSize inSize, void* inMapped)
        {
            auto* data   = new VulkanRHIBufferData();
            data->buffer = inBuffer;
            data->size   = inSize;
            data->bHost  = inMapped != nullptr;
            data->bOwned = false;
            data->mapped = inMapped;
            return {data};
        }

        RHI::Image VulkanRHIDevice::wrapImage(
            const VulkanImageInfo& inInfo, RHI::ImageKind inKind, RHI::ImageFormat inFormat, std::uint32_t inLayers
        )
        {
            auto* data   = new VulkanRHIImageData();
            data->info   = inInfo;
            data->kind   = inKind;
            data->format = inFormat;
            data->layers = inLayers;
            data->bOwned = false;
            data->layout = rememberedLayout(data);
            return {data};
        }

        RHI::BindGroup VulkanRHIDevice::wrapDescriptorSet(vk::DescriptorSet inSet)
        {
            auto* data   = new VulkanRHIGroupData();
            data->set    = inSet;
            data->bOwned = false;
            return {data};
        }

        RHI::CommandList* VulkanRHIDevice::commandList()
        {
            return m_commands.get();
        }

        void VulkanRHIDevice::setCommandBuffer(vk::CommandBuffer inCommandBuffer)
        {
            m_commands->setCommandBuffer(inCommandBuffer);
        }
    }
}
