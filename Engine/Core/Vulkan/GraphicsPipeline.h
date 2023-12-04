#pragma once

#include "Base.h"

namespace Engine
{
	namespace Core
	{
		namespace Vulkan
		{
			namespace GraphicsPipeline
			{
				struct CreateInfo
				{
					vk::Device logicalDevice;
					std::string vertexShaderName;
					std::string fragmentShaderName;
					vk::Extent2D swapChainExtent;
					vk::Format swapChainImageFormat;
				};

				struct Bundle
				{
					vk::PipelineLayout layout;
					vk::RenderPass renderPass;
					vk::Pipeline pipeline;
				};
			}
		}
	}
}