#pragma once

#include "Base.hpp"

#include "SwapChain.hpp"

namespace Engine
{
	namespace Core
	{
		namespace Render
		{
			namespace Buffer
			{
				struct FramebufferCreateInfo
				{
					vk::Device logicalDevice;
					vk::RenderPass renderPass;
					vk::Extent2D swapChainExtent;
					std::vector<SwapChain::Frame>& frames;
				};

				struct CommandBufferCreateInfo
				{
					vk::Device logicalDevice;
					vk::CommandPool commandPool;
					std::vector<SwapChain::Frame>& frames;
				};
			}
		}
	}
}