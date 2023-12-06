#pragma once

#include "Base.hpp"

#include "Classes/GraphicsPipeline.hpp"

#include "Shader.hpp"

namespace Engine
{
	namespace Core
	{
		namespace Render
		{
			namespace GraphicsPipeline
			{
				vk::PipelineVertexInputStateCreateInfo createVertexInputState();
				vk::PipelineInputAssemblyStateCreateInfo createInputAssemblyState();
				vk::PipelineShaderStageCreateInfo createVertexShader(
					vk::ShaderModule& inShaderModule,
					GraphicsPipeline::CreateInfo& inCreateInfo
				);
				vk::PipelineViewportStateCreateInfo createViewport(
					GraphicsPipeline::CreateInfo& inCreateInfo,
					vk::Viewport& inViewport,
					vk::Rect2D& inScissor
				);
				vk::PipelineRasterizationStateCreateInfo createRasterizerState();
				vk::PipelineShaderStageCreateInfo createFragmentShader(
					vk::ShaderModule& inShaderModule,
					GraphicsPipeline::CreateInfo& inCreateInfo
				);
				vk::PipelineMultisampleStateCreateInfo createMulitsampleState();
				vk::PipelineColorBlendStateCreateInfo createColorBlendState(vk::PipelineColorBlendAttachmentState& colorBlendAttachmentState);
				vk::PipelineLayout createLayout(GraphicsPipeline::CreateInfo& inCreateInfo);
				vk::RenderPass createRendepass(GraphicsPipeline::CreateInfo& inCreateInfo);

				void init(GraphicsPipeline::Bundle& allocator, GraphicsPipeline::CreateInfo& inCreateInfo);
			}
		}
	}
}