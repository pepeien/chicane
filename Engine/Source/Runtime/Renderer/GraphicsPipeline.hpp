#pragma once

#include "Base.hpp"

#include "GraphicsPipeline.hpp"
#include "Vertex.hpp"
#include "Shader.hpp"

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
					vk::Pipeline instance;
				};

				vk::PipelineVertexInputStateCreateInfo createVertexInputState(
					vk::VertexInputBindingDescription& inBindingDescription,
					std::array<vk::VertexInputAttributeDescription, 2>& inAttributeDescriptions
				);
				vk::PipelineInputAssemblyStateCreateInfo createInputAssemblyState();
				vk::PipelineShaderStageCreateInfo createVertexShader(
					vk::ShaderModule& inShaderModule,
					CreateInfo& inCreateInfo
				);
				vk::PipelineViewportStateCreateInfo createViewport(
					CreateInfo& inCreateInfo,
					vk::Viewport& inViewport,
					vk::Rect2D& inScissor
				);
				vk::PipelineRasterizationStateCreateInfo createRasterizerState();
				vk::PipelineShaderStageCreateInfo createFragmentShader(
					vk::ShaderModule& inShaderModule,
					CreateInfo& inCreateInfo
				);
				vk::PipelineMultisampleStateCreateInfo createMulitsampleState();
				vk::PipelineColorBlendStateCreateInfo createColorBlendState(vk::PipelineColorBlendAttachmentState& inColorBlendAttachmentState);
				vk::PipelineLayout createLayout(CreateInfo& inCreateInfo);
				vk::RenderPass createRendepass(CreateInfo& inCreateInfo);

				void init(Bundle& outGraphicsPipeline, CreateInfo& inCreateInfo);
			}
		}
	}
}