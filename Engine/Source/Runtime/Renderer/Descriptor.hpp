#pragma once

#include "Base.hpp"

namespace Engine
{
	namespace Descriptor
	{
		struct PoolCreateInfo
		{
			uint32_t count;
			uint32_t size;
			std::vector<vk::DescriptorType> types;
		};

		struct SetLayoutBidingsCreateInfo
		{
			uint32_t count;
			std::vector<uint32_t> indices;
			std::vector<vk::DescriptorType> types;
			std::vector<uint32_t> counts;
			std::vector<vk::ShaderStageFlags> stages;
		};

		struct Bundle
		{
			vk::DescriptorSetLayout setLayout;
			vk::DescriptorSet set;
			vk::DescriptorPool pool;
		};

		void initPool(
			vk::DescriptorPool& outDescriptorPool,
			const vk::Device& inLogicalDevice,
			const PoolCreateInfo& inCreateInfo
		);

		void initSetLayout(
			vk::DescriptorSetLayout& outDescriptorSetLayout,
			const vk::Device& inLogicalDevice,
			const SetLayoutBidingsCreateInfo& inBidingsCreateInfo
		);

		void initSet(
			vk::DescriptorSet& outDescriptorSet,
			const vk::Device& inLogicalDevice,
			const vk::DescriptorSetLayout& inLayout,
			const vk::DescriptorPool& inPool
		);
	}
}