#include "Instance.hpp"

namespace Engine
{
    namespace Texture
    {
		Instance::Instance(const CreateInfo& inCreateInfo)
		{
			width                = inCreateInfo.width;
			height               = inCreateInfo.height;
			filename             = FileSystem::getRelativeTexturePath(inCreateInfo.filename);
			logicalDevice        = inCreateInfo.logicalDevice;
			physicalDevice       = inCreateInfo.physicalDevice;
			commandBuffer        = inCreateInfo.commandBuffer;
			queue                = inCreateInfo.queue;
			descriptor.setLayout = inCreateInfo.descriptorSetLayout;
			descriptor.pool      = inCreateInfo.descriptorPool;

			load();

			Image::CreateInfo imageCreateInfo;
			imageCreateInfo.width            = width;
			imageCreateInfo.height           = height;
			imageCreateInfo.filename         = filename;
			imageCreateInfo.logicalDevice    = logicalDevice;
			imageCreateInfo.physicalDevice   = physicalDevice;
			imageCreateInfo.tiling           = vk::ImageTiling::eOptimal;
			imageCreateInfo.usage            = vk::ImageUsageFlagBits::eTransferDst | vk::ImageUsageFlagBits::eSampled;
			imageCreateInfo.memoryProperties = vk::MemoryPropertyFlagBits::eDeviceLocal;

			Image::init(image.instance, imageCreateInfo);
			Image::initMemory(image.memory, imageCreateInfo, image.instance);

			populate();

			free(pixels);

			makeView();
			makeSampler();
			makeDescriptorSet();
		}

		Instance::~Instance()
		{
			logicalDevice.free(image.memory);
			logicalDevice.destroyImage(image.instance);
			logicalDevice.destroyImageView(image.view);
			logicalDevice.destroySampler(image.sampler);
		}

		void Instance::use(const vk::CommandBuffer& inCommandBuffer, const vk::PipelineLayout& inPipelineLayout)
		{
			inCommandBuffer.bindDescriptorSets(
				vk::PipelineBindPoint::eGraphics,
				inPipelineLayout,
				1,
				descriptor.set,
				nullptr
			);
		}

		void Instance::load()
		{
			pixels = stbi_load(filename.c_str(), &width, &height, &channels, STBI_rgb_alpha);

			if (pixels == nullptr)
			{
				throw std::runtime_error("Failed to load the pixels for " + filename);
			}
		}

		void Instance::populate()
		{
			Vertex::BufferCreateInfo stagingBufferCreateInfo;
			stagingBufferCreateInfo.logicalDevice    = logicalDevice;
			stagingBufferCreateInfo.physicalDevice   = physicalDevice;
			stagingBufferCreateInfo.memoryProperties = vk::MemoryPropertyFlagBits::eHostCoherent | vk::MemoryPropertyFlagBits::eHostVisible;
			stagingBufferCreateInfo.usage            = vk::BufferUsageFlagBits::eTransferSrc;
			stagingBufferCreateInfo.size             = sizeof(float) * (width * height);

			Vertex::Buffer stagingBuffer;
			Vertex::initBuffer(stagingBuffer, stagingBufferCreateInfo);

			void* statingWriteLocation = logicalDevice.mapMemory(stagingBuffer.memory, 0, stagingBufferCreateInfo.size);
			memcpy(statingWriteLocation, pixels, stagingBufferCreateInfo.size);
			logicalDevice.unmapMemory(stagingBuffer.memory);

			Image::transitionLayout(
				commandBuffer,
				queue,
				image.instance,
				vk::ImageLayout::eUndefined,
				vk::ImageLayout::eTransferDstOptimal
			);

			Image::copyBufferToImage(
				commandBuffer,
				queue,
				stagingBuffer.instance,
				image.instance,
				width,
				height
			);

			Image::transitionLayout(
				commandBuffer,
				queue,
				image.instance,
				vk::ImageLayout::eTransferDstOptimal,
				vk::ImageLayout::eShaderReadOnlyOptimal
			);

			Vertex::destroyBuffer(logicalDevice, stagingBuffer);
		}

		void Instance::makeView()
		{
			Image::initView(
				image.view,
				logicalDevice,
				image.instance,
				vk::Format::eR8G8B8A8Unorm
			);
		}

		void Instance::makeSampler()
		{
			vk::SamplerCreateInfo createInfo;
			createInfo.flags                   = vk::SamplerCreateFlags();
			createInfo.minFilter               = vk::Filter::eNearest;
			createInfo.magFilter               = vk::Filter::eLinear;
			createInfo.addressModeU            = vk::SamplerAddressMode::eRepeat;
			createInfo.addressModeV            = vk::SamplerAddressMode::eRepeat;
			createInfo.addressModeW            = vk::SamplerAddressMode::eRepeat;
			createInfo.anisotropyEnable        = VK_FALSE;
			createInfo.maxAnisotropy           = 1.0f;
			createInfo.borderColor             = vk::BorderColor::eIntOpaqueBlack;
			createInfo.unnormalizedCoordinates = VK_FALSE;
			createInfo.compareEnable           = VK_FALSE;
			createInfo.compareOp               = vk::CompareOp::eAlways;
			createInfo.mipmapMode              = vk::SamplerMipmapMode::eLinear;
			createInfo.mipLodBias              = 0.0f;
			createInfo.minLod                  = 0.0f;
			createInfo.maxLod                  = 0.0f;

			image.sampler = logicalDevice.createSampler(createInfo);
		}

		void Instance::makeDescriptorSet()
		{
			Descriptor::initSet(
				descriptor.set,
				logicalDevice,
				descriptor.setLayout,
				descriptor.pool
			);

			vk::DescriptorImageInfo imageDescriptorInfo;
			imageDescriptorInfo.imageLayout = vk::ImageLayout::eShaderReadOnlyOptimal;
			imageDescriptorInfo.imageView   = image.view;
			imageDescriptorInfo.sampler     = image.sampler;

			vk::WriteDescriptorSet imageWriteDescriptorSet;
			imageWriteDescriptorSet.dstSet          = descriptor.set;
			imageWriteDescriptorSet.dstBinding      = 0;
			imageWriteDescriptorSet.dstArrayElement = 0;
			imageWriteDescriptorSet.descriptorType  = vk::DescriptorType::eCombinedImageSampler;
			imageWriteDescriptorSet.descriptorCount = 1;
			imageWriteDescriptorSet.pImageInfo      = &imageDescriptorInfo;

			logicalDevice.updateDescriptorSets(imageWriteDescriptorSet, nullptr);
		}
	}
}