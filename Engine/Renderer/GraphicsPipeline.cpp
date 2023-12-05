#include "GraphicsPipeline.hpp"

namespace Engine
{
	namespace Core
	{
		namespace Render
		{
			namespace GraphicsPipeline
			{
				vk::PipelineVertexInputStateCreateInfo createVertexInputState()
				{
					vk::PipelineVertexInputStateCreateInfo vertexInputInfo = {};
					vertexInputInfo.flags                           = vk::PipelineVertexInputStateCreateFlags();
					vertexInputInfo.vertexBindingDescriptionCount   = 0;
					vertexInputInfo.vertexAttributeDescriptionCount = 0;
				
					return vertexInputInfo;
				}
				
				vk::PipelineInputAssemblyStateCreateInfo createInputAssemblyState()
				{
					vk::PipelineInputAssemblyStateCreateInfo inputAssemblyInfo = {};
					inputAssemblyInfo.flags    = vk::PipelineInputAssemblyStateCreateFlags();
					inputAssemblyInfo.topology = vk::PrimitiveTopology::eTriangleList;
				
					return inputAssemblyInfo;
				}
				
				vk::PipelineShaderStageCreateInfo createVertexShader(
					vk::ShaderModule& inShaderModule,
					GraphicsPipeline::CreateInfo& inCreateInfo
				)
				{
					vk::PipelineShaderStageCreateInfo vertexShaderInfo = {};
					vertexShaderInfo.flags  = vk::PipelineShaderStageCreateFlags();
					vertexShaderInfo.stage  = vk::ShaderStageFlagBits::eVertex;
					vertexShaderInfo.module = inShaderModule;
					vertexShaderInfo.pName  = "main";

					return vertexShaderInfo;
				}

				vk::PipelineViewportStateCreateInfo createViewport(
					GraphicsPipeline::CreateInfo& inCreateInfo,
					vk::Viewport& inViewport,
					vk::Rect2D& inScissor
				)
				{
					vk::PipelineViewportStateCreateInfo viewportState = {};
					viewportState.flags         = vk::PipelineViewportStateCreateFlags();
					viewportState.viewportCount = 1;
					viewportState.pViewports    = &inViewport;
					viewportState.scissorCount  = 1;
					viewportState.pScissors     = &inScissor;

					return viewportState;
				}

				vk::PipelineRasterizationStateCreateInfo createRasterizerState()
				{
					vk::PipelineRasterizationStateCreateInfo rasterizationState = {};
					rasterizationState.flags                   = vk::PipelineRasterizationStateCreateFlags();
					rasterizationState.depthClampEnable        = VK_FALSE;
					rasterizationState.rasterizerDiscardEnable = VK_FALSE;
					rasterizationState.polygonMode             = vk::PolygonMode::eFill;
					rasterizationState.lineWidth               = 1.0f;
					rasterizationState.cullMode                = vk::CullModeFlagBits::eBack;
					rasterizationState.frontFace               = vk::FrontFace::eClockwise;
					rasterizationState.depthBiasEnable         = VK_FALSE;

					return rasterizationState;
				}

				vk::PipelineShaderStageCreateInfo createFragmentShader(
					vk::ShaderModule& inShaderModule,
					GraphicsPipeline::CreateInfo& inCreateInfo
				)
				{
					vk::PipelineShaderStageCreateInfo fragmentShaderInfo = {};
					fragmentShaderInfo.flags  = vk::PipelineShaderStageCreateFlags();
					fragmentShaderInfo.stage  = vk::ShaderStageFlagBits::eFragment;
					fragmentShaderInfo.module = inShaderModule;
					fragmentShaderInfo.pName  = "main";

					return fragmentShaderInfo;
				}

				vk::PipelineMultisampleStateCreateInfo createMulitsampleState()
				{
					vk::PipelineMultisampleStateCreateInfo multisampleState = {};
					multisampleState.flags                = vk::PipelineMultisampleStateCreateFlags();
					multisampleState.sampleShadingEnable  = VK_FALSE;
					multisampleState.rasterizationSamples = vk::SampleCountFlagBits::e1;

					return multisampleState;
				}

				vk::PipelineColorBlendStateCreateInfo createColorBlendState(vk::PipelineColorBlendAttachmentState& colorBlendAttachmentState)
				{
					vk::PipelineColorBlendStateCreateInfo colorBlendState = {};
					colorBlendState.flags             = vk::PipelineColorBlendStateCreateFlags();
					colorBlendState.logicOpEnable     = VK_FALSE;
					colorBlendState.logicOp           = vk::LogicOp::eCopy;
					colorBlendState.attachmentCount   = 1;
					colorBlendState.pAttachments      = &colorBlendAttachmentState;
					colorBlendState.blendConstants[0] = 0.0f;
					colorBlendState.blendConstants[1] = 0.0f;
					colorBlendState.blendConstants[2] = 0.0f;
					colorBlendState.blendConstants[3] = 0.0f;

					return colorBlendState;
				}

				vk::PipelineLayout createLayout(GraphicsPipeline::CreateInfo& inCreateInfo)
				{
					vk::PipelineLayoutCreateInfo layoutCreateInfo = {};
					layoutCreateInfo.flags                  = vk::PipelineLayoutCreateFlags();
					layoutCreateInfo.setLayoutCount         = 0;
					layoutCreateInfo.pushConstantRangeCount = 0;

					return inCreateInfo.logicalDevice.createPipelineLayout(layoutCreateInfo);
				}

				vk::RenderPass createRendepass(GraphicsPipeline::CreateInfo& inCreateInfo)
				{
					vk::AttachmentDescription colorAttachment = {};
					colorAttachment.flags          = vk::AttachmentDescriptionFlags();
					colorAttachment.format         = inCreateInfo.swapChainImageFormat;
					colorAttachment.samples        = vk::SampleCountFlagBits::e1;
					colorAttachment.loadOp         = vk::AttachmentLoadOp::eClear;
					colorAttachment.storeOp        = vk::AttachmentStoreOp::eStore;
					colorAttachment.stencilLoadOp  = vk::AttachmentLoadOp::eDontCare;
					colorAttachment.stencilStoreOp = vk::AttachmentStoreOp::eDontCare;
					colorAttachment.initialLayout  = vk::ImageLayout::eUndefined;
					colorAttachment.finalLayout    = vk::ImageLayout::ePresentSrcKHR;

					vk::AttachmentReference colorAttachmentRef = {};
					colorAttachmentRef.attachment = 0;
					colorAttachmentRef.layout     = vk::ImageLayout::eColorAttachmentOptimal;

					vk::SubpassDescription subpass = {};
					subpass.flags                = vk::SubpassDescriptionFlags();
					subpass.pipelineBindPoint    = vk::PipelineBindPoint::eGraphics;
					subpass.colorAttachmentCount = 1;
					subpass.pColorAttachments    = &colorAttachmentRef;

					vk::RenderPassCreateInfo renderPassInfo = {};
					renderPassInfo.flags           = vk::RenderPassCreateFlags();
					renderPassInfo.attachmentCount = 1;
					renderPassInfo.pAttachments    = &colorAttachment;
					renderPassInfo.subpassCount    = 1;
					renderPassInfo.pSubpasses      = &subpass;

					return inCreateInfo.logicalDevice.createRenderPass(renderPassInfo);
				}

				void init(GraphicsPipeline::Bundle& allocator, GraphicsPipeline::CreateInfo& inCreateInfo)
				{
					vk::ShaderModule vertexShaderModule;
					Shader::initModule(vertexShaderModule, inCreateInfo.vertexShaderName, inCreateInfo.logicalDevice);

					vk::ShaderModule fragmentShaderModule;
					Shader::initModule(fragmentShaderModule, inCreateInfo.fragmentShaderName, inCreateInfo.logicalDevice);

					vk::Viewport viewport = {};
					viewport.x        = 0;
					viewport.y        = 0;
					viewport.width    = static_cast<float>(inCreateInfo.swapChainExtent.width);
					viewport.height   = static_cast<float>(inCreateInfo.swapChainExtent.height);
					viewport.minDepth = 0.0f;
					viewport.maxDepth = 1.0f;

					vk::Rect2D scissor = {};
					scissor.offset.x = 0;
					scissor.offset.y = 0;
					scissor.extent   = inCreateInfo.swapChainExtent;

					vk::PipelineColorBlendAttachmentState colorBlendAttachmentState = {};
					colorBlendAttachmentState.colorWriteMask = vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG |
															   vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA;
					colorBlendAttachmentState.blendEnable    = VK_FALSE;

					vk::PipelineVertexInputStateCreateInfo vertexInputState     = createVertexInputState();
					vk::PipelineInputAssemblyStateCreateInfo inputAssemblyState = createInputAssemblyState();
					vk::PipelineShaderStageCreateInfo vertexShaderCreateInfo    = createVertexShader(vertexShaderModule, inCreateInfo);
					vk::PipelineViewportStateCreateInfo viewportState           = createViewport(inCreateInfo, viewport, scissor);
					vk::PipelineRasterizationStateCreateInfo rasterizationState = createRasterizerState();
					vk::PipelineShaderStageCreateInfo fragmentShaderCreateInfo  = createFragmentShader(fragmentShaderModule, inCreateInfo);
					vk::PipelineMultisampleStateCreateInfo multisampleState     = createMulitsampleState();
					vk::PipelineColorBlendStateCreateInfo colorBlendState       = createColorBlendState(colorBlendAttachmentState);
					vk::PipelineLayout layout                                   = createLayout(inCreateInfo);
					vk::RenderPass renderPass                                   = createRendepass(inCreateInfo);

					std::vector<vk::PipelineShaderStageCreateInfo> shaderStages;
					shaderStages.push_back(vertexShaderCreateInfo);
					shaderStages.push_back(fragmentShaderCreateInfo);

					vk::GraphicsPipelineCreateInfo pipelineInfo = {};
					pipelineInfo.flags               = vk::PipelineCreateFlags();
					pipelineInfo.pVertexInputState   = &vertexInputState;
					pipelineInfo.pInputAssemblyState = &inputAssemblyState;
					pipelineInfo.pViewportState      = &viewportState;
					pipelineInfo.pRasterizationState = &rasterizationState;
					pipelineInfo.stageCount          = static_cast<uint32_t>(shaderStages.size());
					pipelineInfo.pStages             = shaderStages.data();
					pipelineInfo.pMultisampleState   = &multisampleState;
					pipelineInfo.pColorBlendState    = &colorBlendState;
					pipelineInfo.layout              = layout;
					pipelineInfo.renderPass          = renderPass;
					pipelineInfo.subpass             = 0;
					pipelineInfo.basePipelineHandle  = nullptr;

					allocator.layout     = layout;
					allocator.renderPass = renderPass;
					allocator.pipeline   = inCreateInfo.logicalDevice.createGraphicsPipeline(nullptr, pipelineInfo).value;

					inCreateInfo.logicalDevice.destroyShaderModule(vertexShaderModule);
					inCreateInfo.logicalDevice.destroyShaderModule(fragmentShaderModule);
				}
			}
		}
	}
}