#pragma once

#include "Base.hpp"

namespace Engine
{
	namespace Runtime
	{
		namespace Renderer
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