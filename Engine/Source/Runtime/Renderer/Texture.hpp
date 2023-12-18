#pragma once

#include "Base.hpp"

#include "Vertex.hpp"
#include "Image.hpp"
#include "Descriptor.hpp"

namespace Engine
{
    namespace Texture
    {
		struct CreateInfo
		{
			uint32_t width;
			uint32_t height;
			std::string filename;
			vk::Device logicalDevice;
			vk::PhysicalDevice physicalDevice;
			vk::CommandBuffer commandBuffer;
			vk::Queue queue;
			vk::DescriptorSetLayout descriptorSetLayout;
			vk::DescriptorPool descriptorPool;
		};

		struct ImageBundle
		{
			vk::Image instance;
			vk::DeviceMemory memory;
			vk::ImageView view;
			vk::Sampler sampler;
		};

		struct DescriptorBundle
		{
			vk::DescriptorSetLayout setLayout;
			vk::DescriptorSet set;
			vk::DescriptorPool pool;
		};

		class Instance
		{
		public:
			Instance(const CreateInfo& inCreateInfo);
			~Instance();

		public:
			void use(const vk::CommandBuffer& inCommandBuffer, const vk::PipelineLayout& inPipelineLayout);

		private:
			void load();
			void populate();
			void makeView();
			void makeSampler();
			void makeDescriptorSet();

		private:
			int width;
			int height;
			int channels;
			std::string filename;
			vk::Device logicalDevice;
			vk::PhysicalDevice physicalDevice;
			stbi_uc* pixels;

			ImageBundle image;

			DescriptorBundle descriptor;

			vk::CommandBuffer commandBuffer;
			vk::Queue queue;
		};
	}
}