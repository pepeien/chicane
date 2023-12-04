#pragma once

#include "Base.h"

#include "Core/Vulkan/GraphicsPipeline.h"

namespace Engine
{
	namespace Core
	{
		namespace Vulkan
		{
			namespace Mounter
			{
				vk::PipelineVertexInputStateCreateInfo createGraphicsPipelineVertexInputState();
				vk::PipelineInputAssemblyStateCreateInfo createGraphicsPipelineInputAssemblyState();
				vk::PipelineShaderStageCreateInfo createGraphicsPipelineVertexShader(
					vk::ShaderModule& inShaderModule,
					GraphicsPipeline::CreateInfo& inCreateInfo
				);
				vk::PipelineViewportStateCreateInfo createGraphicsPipelineViewport(
					GraphicsPipeline::CreateInfo& inCreateInfo,
					vk::Viewport& inViewport,
					vk::Rect2D& inScissor
				);
				vk::PipelineRasterizationStateCreateInfo createGraphicsPipelineRasterizerState();
				vk::PipelineShaderStageCreateInfo createGraphicsPipelineFragmentShader(
					vk::ShaderModule& inShaderModule,
					GraphicsPipeline::CreateInfo& inCreateInfo
				);
				vk::PipelineMultisampleStateCreateInfo createGraphicsPipelineMulitsampleState();
				vk::PipelineColorBlendStateCreateInfo createGraphicsPipelineColorBlendState();
				vk::PipelineLayout createGraphicsPipelineLayout(GraphicsPipeline::CreateInfo& inCreateInfo);
				vk::RenderPass createGraphicsPipelineRendepass(GraphicsPipeline::CreateInfo& inCreateInfo);

				void initGraphicsPipeline(GraphicsPipeline::Bundle& allocator, GraphicsPipeline::CreateInfo& inCreateInfo);
			}
		}
	}
}