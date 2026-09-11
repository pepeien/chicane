#include "Chicane/Renderer/Backend/Vulkan/Layer/Scene/Line.hpp"

#include <algorithm>
#include <cstddef>
#include <cstring>

#include "Chicane/Renderer/Backend/Vulkan.hpp"
#include "Chicane/Renderer/Backend/Vulkan/Buffer.hpp"
#include "Chicane/Renderer/Backend/Vulkan/Buffer/CreateInfo.hpp"
#include "Chicane/Renderer/Backend/Vulkan/Descriptor/Pool.hpp"
#include "Chicane/Renderer/Backend/Vulkan/Descriptor/Pool/CreateInfo.hpp"
#include "Chicane/Renderer/Backend/Vulkan/Descriptor/SetLayout.hpp"
#include "Chicane/Renderer/Backend/Vulkan/Descriptor/SetLayout/BidingsCreateInfo.hpp"
#include "Chicane/Renderer/Backend/Vulkan/Frame.hpp"
#include "Chicane/Renderer/Backend/Vulkan/GraphicsPipeline/Builder.hpp"
#include "Chicane/Renderer/Backend/Vulkan/Layer/Scene.hpp"
#include "Chicane/Renderer/Backend/Vulkan/Vertex.hpp"
#include "Chicane/Renderer/Debug/Mode.hpp"
#include "Chicane/Renderer/Draw/Poly.hpp"
#include "Chicane/Renderer/Draw/Poly/3D/Flag.hpp"
#include "Chicane/Renderer/Draw/Poly/Topology.hpp"
#include "Chicane/Renderer/Instance.hpp"

namespace Chicane
{
    namespace Renderer
    {
        VulkanLSceneLine::VulkanLSceneLine()
            : Layer(SCENE_LINE_LAYER_ID),
              m_clear({vk::ClearColorValue(0.0f, 0.0f, 0.0f, 0.0f), vk::ClearDepthStencilValue(1.0f, 0)})
        {}

        void VulkanLSceneLine::onInit()
        {
            initFrameResources();
            initMeshGraphicsPipeline();
            initLineListGraphicsPipeline(m_lineListPipeline, true);
            initLineListGraphicsPipeline(m_lineListForegroundPipeline, false);
            initOutlineGraphicsPipeline();
            initFramebuffers();
        }

        void VulkanLSceneLine::onRestart()
        {
            initFramebuffers();
        }

        void VulkanLSceneLine::onDestruction()
        {
            getBackend<VulkanBackend>()->logicalDevice.waitIdle();

            destroyImmediateBuffers();
            destroyFrameResources();

            if (m_outlineMaskPipeline.instance)
            {
                getBackend<VulkanBackend>()->logicalDevice.destroyPipeline(m_outlineMaskPipeline.instance);
                getBackend<VulkanBackend>()->logicalDevice.destroyPipelineLayout(m_outlineMaskPipeline.layout);
                m_outlineMaskPipeline.instance   = nullptr;
                m_outlineMaskPipeline.layout     = nullptr;
                m_outlineMaskPipeline.renderPass = nullptr;
            }

            if (m_outlinePipeline.instance)
            {
                getBackend<VulkanBackend>()->logicalDevice.destroyPipeline(m_outlinePipeline.instance);
                getBackend<VulkanBackend>()->logicalDevice.destroyPipelineLayout(m_outlinePipeline.layout);
                m_outlinePipeline.instance   = nullptr;
                m_outlinePipeline.layout     = nullptr;
                m_outlinePipeline.renderPass = nullptr;
            }

            if (m_lineListForegroundPipeline.instance)
            {
                getBackend<VulkanBackend>()->logicalDevice.destroyPipeline(m_lineListForegroundPipeline.instance);
                getBackend<VulkanBackend>()->logicalDevice.destroyPipelineLayout(m_lineListForegroundPipeline.layout);
                m_lineListForegroundPipeline.instance   = nullptr;
                m_lineListForegroundPipeline.layout     = nullptr;
                m_lineListForegroundPipeline.renderPass = nullptr;
            }

            if (m_lineListPipeline.instance)
            {
                getBackend<VulkanBackend>()->logicalDevice.destroyPipeline(m_lineListPipeline.instance);
                getBackend<VulkanBackend>()->logicalDevice.destroyPipelineLayout(m_lineListPipeline.layout);
                m_lineListPipeline.instance   = nullptr;
                m_lineListPipeline.layout     = nullptr;
                m_lineListPipeline.renderPass = nullptr;
            }

            m_meshPipeline.destroy();
        }

        bool VulkanLSceneLine::shouldDrawMeshWireframe(const Frame& inFrame) const
        {
            const Instance* renderer = getBackend()->getRenderer();
            if (renderer->hasDebug(DebugMode::Meshes) && inFrame.hasDraws(DrawPolyType::e3D, DrawPolyMode::Fill))
            {
                return true;
            }

            for (const DrawPoly& draw : inFrame.getDraws(DrawPolyType::e3D, DrawPolyMode::Line))
            {
                if (!draw.isLineList())
                {
                    return true;
                }
            }

            return false;
        }

        bool VulkanLSceneLine::shouldDrawLineList(const Frame& inFrame) const
        {
            for (const DrawPoly& draw : inFrame.getDraws(DrawPolyType::e3D, DrawPolyMode::Line))
            {
                if (draw.isLineList())
                {
                    return true;
                }
            }

            return false;
        }

        bool VulkanLSceneLine::shouldDrawOutline(const Frame& inFrame) const
        {
            return inFrame.hasOutlineDraws();
        }

        bool VulkanLSceneLine::onBeginRender(const Frame& inFrame)
        {
            return shouldDrawMeshWireframe(inFrame) || shouldDrawLineList(inFrame) || shouldDrawOutline(inFrame);
        }

        void VulkanLSceneLine::onRender(const Frame& inFrame, void* inData)
        {
            VulkanBackend* backend = getBackend<VulkanBackend>();
            VulkanLScene*  parent  = backend->getLayer<VulkanLScene>(SCENE_LAYER_ID);
            if (!parent)
            {
                return;
            }

            const Instance*   renderer      = backend->getRenderer();
            VulkanFrame&      frame         = *((VulkanFrame*)inData);
            vk::CommandBuffer commandBuffer = frame.commandBuffer;

            vk::Viewport viewport = backend->getVkViewport(this);
            commandBuffer.setViewport(0, 1, &viewport);

            vk::Rect2D scissor = backend->getVkScissor(this);
            commandBuffer.setScissor(0, 1, &scissor);
            commandBuffer.setLineWidth(1.0f);

            vk::RenderPassBeginInfo beginInfo;
            beginInfo.renderPass               = m_meshPipeline.renderPass;
            beginInfo.framebuffer              = frame.image.getFramebuffer(m_id);
            beginInfo.renderArea.extent.width  = viewport.width;
            beginInfo.renderArea.extent.height = viewport.height;
            beginInfo.clearValueCount          = static_cast<std::uint32_t>(m_clear.size());
            beginInfo.pClearValues             = m_clear.data();

            commandBuffer.beginRenderPass(&beginInfo, vk::SubpassContents::eInline);

            if (shouldDrawMeshWireframe(inFrame))
            {
                m_meshPipeline.bind(commandBuffer);
                m_meshPipeline.bind(commandBuffer, 0, frame.getDescriptorSet(m_id));
                commandBuffer.setLineWidth(1.0f);

                vk::Buffer     vertexBuffers[] = {parent->modelVertexBuffer.instance};
                vk::DeviceSize offsets[]       = {0};
                commandBuffer.bindVertexBuffers(0, 1, vertexBuffers, offsets);
                commandBuffer.bindIndexBuffer(parent->modelIndexBuffer.instance, 0, vk::IndexType::eUint32);

                auto drawBatch = [&](DrawPolyMode inMode)
                {
                    for (const DrawPoly& draw : inFrame.getDraws(DrawPolyType::e3D, inMode))
                    {
                        if (draw.isLineList())
                        {
                            continue;
                        }

                        commandBuffer.drawIndexed(
                            draw.indexCount,
                            draw.instanceCount,
                            draw.indexStart,
                            draw.vertexStart,
                            draw.instanceStart
                        );
                    }
                };

                if (inFrame.hasDraws(DrawPolyType::e3D, DrawPolyMode::Line))
                {
                    drawBatch(DrawPolyMode::Line);
                }

                if (renderer->hasDebug(DebugMode::Meshes))
                {
                    for (const DrawPoly& draw : inFrame.getSceneDraws())
                    {
                        commandBuffer.drawIndexed(
                            draw.indexCount,
                            draw.instanceCount,
                            draw.indexStart,
                            draw.vertexStart,
                            draw.instanceStart
                        );
                    }
                }
            }

            if (shouldDrawOutline(inFrame))
            {
                m_outlineMaskPipeline.bind(commandBuffer);
                m_outlineMaskPipeline.bind(commandBuffer, 0, frame.getDescriptorSet(m_id));

                vk::Buffer     vertexBuffers[] = {parent->modelVertexBuffer.instance};
                vk::DeviceSize offsets[]       = {0};
                commandBuffer.bindVertexBuffers(0, 1, vertexBuffers, offsets);
                commandBuffer.bindIndexBuffer(parent->modelIndexBuffer.instance, 0, vk::IndexType::eUint32);

                drawOutlineMeshes(inFrame, commandBuffer, m_outlineMaskPipeline.layout, 0.0f, 0.0f);

                m_outlinePipeline.bind(commandBuffer);
                m_outlinePipeline.bind(commandBuffer, 0, frame.getDescriptorSet(m_id));
                commandBuffer.bindVertexBuffers(0, 1, vertexBuffers, offsets);
                commandBuffer.bindIndexBuffer(parent->modelIndexBuffer.instance, 0, vk::IndexType::eUint32);

                const float scaleX = viewport.width > 0.0f ? 4.0f / viewport.width : 0.0f;
                const float scaleY = viewport.height > 0.0f ? 4.0f / viewport.height : 0.0f;

                static const float meshesOffsets[8][2] = {
                    {1.0f,         0.0f        },
                    {-1.0f,        0.0f        },
                    {0.0f,         1.0f        },
                    {0.0f,         -1.0f       },
                    {0.70710678f,  0.70710678f },
                    {0.70710678f,  -0.70710678f},
                    {-0.70710678f, 0.70710678f },
                    {-0.70710678f, -0.70710678f}
                };

                for (const float* offset : meshesOffsets)
                {
                    drawOutlineMeshes(
                        inFrame,
                        commandBuffer,
                        m_outlinePipeline.layout,
                        offset[0] * scaleX,
                        offset[1] * scaleY
                    );
                }
            }

            if (shouldDrawLineList(inFrame))
            {
                if (inFrame.hasImmediateVertices())
                {
                    VulkanLSceneLineImmediateBuffer& immediateBuffer = immediateBufferFor(frame);
                    uploadImmediateGeometry(immediateBuffer, inFrame);

                    vk::Buffer     vertexBuffers[] = {immediateBuffer.vertexBuffer.instance};
                    vk::DeviceSize offsets[]       = {0};
                    commandBuffer.bindVertexBuffers(0, 1, vertexBuffers, offsets);

                    for (const DrawPoly& draw : inFrame.getDraws(DrawPolyType::e3D, DrawPolyMode::Line))
                    {
                        if (!draw.isLineList())
                        {
                            continue;
                        }

                        VulkanGraphicsPipeline& pipeline =
                            inFrame.isForegroundDraw(draw) ? m_lineListForegroundPipeline : m_lineListPipeline;
                        pipeline.bind(commandBuffer);
                        pipeline.bind(commandBuffer, 0, frame.getDescriptorSet(m_id));
                        commandBuffer.setLineWidth(1.0f);

                        drawLineList(commandBuffer, draw, immediateBuffer.indexBuffer.instance);
                    }
                }
            }

            commandBuffer.endRenderPass();
        }

        void VulkanLSceneLine::initFrameResources()
        {
            VulkanBackend* backend = getBackend<VulkanBackend>();

            VulkanDescriptorSetLayoutBidingsCreateInfo bidings;
            bidings.count = 2;

            bidings.indices.push_back(0);
            bidings.types.push_back(vk::DescriptorType::eUniformBuffer);
            bidings.counts.push_back(1);
            bidings.stages.push_back(vk::ShaderStageFlagBits::eVertex);

            bidings.indices.push_back(1);
            bidings.types.push_back(vk::DescriptorType::eStorageBuffer);
            bidings.counts.push_back(1);
            bidings.stages.push_back(vk::ShaderStageFlagBits::eVertex);

            VulkanDescriptorSetLayout::init(m_frameDescriptor.setLayout, backend->logicalDevice, bidings);

            VulkanDescriptorPoolCreateInfo descriptorPoolCreateInfo;
            descriptorPoolCreateInfo.maxSets = static_cast<std::uint32_t>(backend->frames.size());
            descriptorPoolCreateInfo.sizes.push_back(
                {vk::DescriptorType::eUniformBuffer, descriptorPoolCreateInfo.maxSets}
            );
            descriptorPoolCreateInfo.sizes.push_back(
                {vk::DescriptorType::eStorageBuffer, descriptorPoolCreateInfo.maxSets}
            );

            VulkanDescriptorPool::init(m_frameDescriptor.pool, backend->logicalDevice, descriptorPoolCreateInfo);

            for (VulkanFrame& frame : backend->frames)
            {
                vk::DescriptorSet descriptorSet;

                VulkanDescriptorSetLayout::allocate(
                    descriptorSet,
                    backend->logicalDevice,
                    m_frameDescriptor.setLayout,
                    m_frameDescriptor.pool
                );
                frame.addDescriptorSet(m_id, descriptorSet);

                vk::WriteDescriptorSet cameraWriteInfo;
                cameraWriteInfo.dstSet          = descriptorSet;
                cameraWriteInfo.dstBinding      = 0;
                cameraWriteInfo.dstArrayElement = 0;
                cameraWriteInfo.descriptorCount = 1;
                cameraWriteInfo.descriptorType  = vk::DescriptorType::eUniformBuffer;
                cameraWriteInfo.pBufferInfo     = &frame.cameraResource.bufferInfo;
                frame.addWriteDescriptorSet(cameraWriteInfo);

                vk::WriteDescriptorSet poly3DWriteInfo;
                poly3DWriteInfo.dstSet          = descriptorSet;
                poly3DWriteInfo.dstBinding      = 1;
                poly3DWriteInfo.dstArrayElement = 0;
                poly3DWriteInfo.descriptorCount = 1;
                poly3DWriteInfo.descriptorType  = vk::DescriptorType::eStorageBuffer;
                poly3DWriteInfo.pBufferInfo     = &frame.poly3DResource.bufferInfo;
                frame.addWriteDescriptorSet(poly3DWriteInfo);
            }
        }

        void VulkanLSceneLine::destroyFrameResources()
        {
            VulkanBackend* backend = getBackend<VulkanBackend>();

            backend->logicalDevice.destroyDescriptorSetLayout(m_frameDescriptor.setLayout);
            backend->logicalDevice.destroyDescriptorPool(m_frameDescriptor.pool);
        }

        void VulkanLSceneLine::initMeshGraphicsPipeline()
        {
            VulkanBackend* backend = getBackend<VulkanBackend>();

            VulkanShaderStageCreateInfo vertexShader;
            vertexShader.path = "Assets/Engine/Shaders/Vulkan/Scene/Line.vvert";
            vertexShader.type = vk::ShaderStageFlagBits::eVertex;

            VulkanShaderStageCreateInfo fragmentShader;
            fragmentShader.path = "Assets/Engine/Shaders/Vulkan/Scene/Line.vfrag";
            fragmentShader.type = vk::ShaderStageFlagBits::eFragment;

            vk::PipelineDepthStencilStateCreateInfo depth;
            depth.depthBoundsTestEnable = VK_FALSE;
            depth.stencilTestEnable     = VK_FALSE;
            depth.depthWriteEnable      = VK_TRUE;
            depth.depthTestEnable       = VK_TRUE;
            depth.depthCompareOp        = vk::CompareOp::eLessOrEqual;
            depth.minDepthBounds        = 0.0f;
            depth.maxDepthBounds        = 1.0f;

            vk::AttachmentDescription colorAttachment;
            colorAttachment.format        = backend->swapchain.colorFormat;
            colorAttachment.samples       = vk::SampleCountFlagBits::e1;
            colorAttachment.loadOp        = vk::AttachmentLoadOp::eLoad;
            colorAttachment.storeOp       = vk::AttachmentStoreOp::eStore;
            colorAttachment.initialLayout = vk::ImageLayout::eColorAttachmentOptimal;
            colorAttachment.finalLayout   = vk::ImageLayout::eColorAttachmentOptimal;

            vk::AttachmentReference colorReference;
            colorReference.attachment = 0;
            colorReference.layout     = vk::ImageLayout::eColorAttachmentOptimal;

            vk::SubpassDependency colorSubpassDepedency;
            colorSubpassDepedency.srcSubpass    = 0;
            colorSubpassDepedency.dstSubpass    = VK_SUBPASS_EXTERNAL;
            colorSubpassDepedency.srcStageMask  = vk::PipelineStageFlagBits::eColorAttachmentOutput;
            colorSubpassDepedency.dstStageMask  = vk::PipelineStageFlagBits::eColorAttachmentOutput;
            colorSubpassDepedency.srcAccessMask = vk::AccessFlagBits::eNone;
            colorSubpassDepedency.dstAccessMask =
                vk::AccessFlagBits::eColorAttachmentRead | vk::AccessFlagBits::eColorAttachmentWrite;

            vk::AttachmentDescription depthAttachment;
            depthAttachment.format         = backend->swapchain.depthFormat;
            depthAttachment.samples        = vk::SampleCountFlagBits::e1;
            depthAttachment.loadOp         = vk::AttachmentLoadOp::eLoad;
            depthAttachment.storeOp        = vk::AttachmentStoreOp::eStore;
            depthAttachment.stencilLoadOp  = vk::AttachmentLoadOp::eClear;
            depthAttachment.stencilStoreOp = vk::AttachmentStoreOp::eDontCare;
            depthAttachment.initialLayout  = vk::ImageLayout::eDepthStencilAttachmentOptimal;
            depthAttachment.finalLayout    = vk::ImageLayout::eDepthStencilAttachmentOptimal;

            vk::AttachmentReference depthReference;
            depthReference.attachment = 1;
            depthReference.layout     = vk::ImageLayout::eDepthStencilAttachmentOptimal;

            vk::SubpassDependency depthSubpassDepedency;
            depthSubpassDepedency.srcSubpass    = 0;
            depthSubpassDepedency.dstSubpass    = VK_SUBPASS_EXTERNAL;
            depthSubpassDepedency.srcStageMask  = vk::PipelineStageFlagBits::eLateFragmentTests;
            depthSubpassDepedency.dstStageMask  = vk::PipelineStageFlagBits::eFragmentShader;
            depthSubpassDepedency.srcAccessMask = vk::AccessFlagBits::eDepthStencilAttachmentWrite;
            depthSubpassDepedency.dstAccessMask = vk::AccessFlagBits::eShaderRead;

            vk::SubpassDescription subpass;
            subpass.pipelineBindPoint       = vk::PipelineBindPoint::eGraphics;
            subpass.colorAttachmentCount    = 1;
            subpass.pColorAttachments       = &colorReference;
            subpass.pDepthStencilAttachment = &depthReference;

            vk::PipelineRasterizationStateCreateInfo rasterization;
            rasterization.depthClampEnable        = VK_FALSE;
            rasterization.rasterizerDiscardEnable = VK_FALSE;
            rasterization.lineWidth               = 1.0f;
            rasterization.depthBiasEnable         = VK_FALSE;
            rasterization.polygonMode             = vk::PolygonMode::eLine;
            rasterization.cullMode                = vk::CullModeFlagBits::eNone;
            rasterization.frontFace               = vk::FrontFace::eCounterClockwise;

            VulkanGraphicsPipelineBuilder()
                .addVertexBinding(VulkanVertex::getBindingDescription())
                .addVertexAttributes(VulkanVertex::getAttributeDescriptions())
                .setInputAssembly(VulkanGraphicsPipeline::createInputAssemblyState())
                .addViewport(backend->getVkViewport(this))
                .addDynamicState(vk::DynamicState::eViewport)
                .addScissor(backend->getVkScissor(this))
                .addDynamicState(vk::DynamicState::eScissor)
                .addDynamicState(vk::DynamicState::eLineWidth)
                .addShaderStage(vertexShader, backend->logicalDevice)
                .addShaderStage(fragmentShader, backend->logicalDevice)
                .addColorBlendingAttachment(VulkanGraphicsPipeline::createBlendAttachmentState(false))
                .addAttachment(colorAttachment)
                .addSubpassDependecy(colorSubpassDepedency)
                .setDepthStencil(depth)
                .addAttachment(depthAttachment)
                .addSubpassDependecy(depthSubpassDepedency)
                .addSubpass(subpass)
                .addDescriptorSetLayout(m_frameDescriptor.setLayout)
                .setRasterization(rasterization)
                .build(m_meshPipeline, backend->logicalDevice);
        }

        void VulkanLSceneLine::initLineListGraphicsPipeline(VulkanGraphicsPipeline& outPipeline, bool inTestDepth)
        {
            VulkanBackend* backend = getBackend<VulkanBackend>();

            VulkanShaderStageCreateInfo vertexShader;
            vertexShader.path = "Assets/Engine/Shaders/Vulkan/Scene/Line.vvert";
            vertexShader.type = vk::ShaderStageFlagBits::eVertex;

            VulkanShaderStageCreateInfo fragmentShader;
            fragmentShader.path = "Assets/Engine/Shaders/Vulkan/Scene/Line.vfrag";
            fragmentShader.type = vk::ShaderStageFlagBits::eFragment;

            vk::PipelineDepthStencilStateCreateInfo depth;
            depth.depthBoundsTestEnable = VK_FALSE;
            depth.stencilTestEnable     = VK_FALSE;
            depth.depthWriteEnable      = inTestDepth ? VK_TRUE : VK_FALSE;
            depth.depthTestEnable       = inTestDepth ? VK_TRUE : VK_FALSE;
            depth.depthCompareOp        = inTestDepth ? vk::CompareOp::eLessOrEqual : vk::CompareOp::eAlways;
            depth.minDepthBounds        = 0.0f;
            depth.maxDepthBounds        = 1.0f;

            vk::PipelineRasterizationStateCreateInfo rasterization;
            rasterization.depthClampEnable        = VK_FALSE;
            rasterization.rasterizerDiscardEnable = VK_FALSE;
            rasterization.lineWidth               = 1.0f;
            rasterization.depthBiasEnable         = VK_FALSE;
            rasterization.polygonMode             = vk::PolygonMode::eFill;
            rasterization.cullMode                = vk::CullModeFlagBits::eNone;
            rasterization.frontFace               = vk::FrontFace::eCounterClockwise;

            vk::PipelineInputAssemblyStateCreateInfo inputAssembly;
            inputAssembly.topology = vk::PrimitiveTopology::eLineList;

            VulkanGraphicsPipelineBuilder()
                .addVertexBinding(VulkanVertex::getBindingDescription())
                .addVertexAttributes(VulkanVertex::getAttributeDescriptions())
                .setInputAssembly(inputAssembly)
                .addViewport(backend->getVkViewport(this))
                .addDynamicState(vk::DynamicState::eViewport)
                .addScissor(backend->getVkScissor(this))
                .addDynamicState(vk::DynamicState::eScissor)
                .addDynamicState(vk::DynamicState::eLineWidth)
                .addShaderStage(vertexShader, backend->logicalDevice)
                .addShaderStage(fragmentShader, backend->logicalDevice)
                .addColorBlendingAttachment(VulkanGraphicsPipeline::createBlendAttachmentState(false))
                .setDepthStencil(depth)
                .addDescriptorSetLayout(m_frameDescriptor.setLayout)
                .setRasterization(rasterization)
                .setRenderPass(m_meshPipeline.renderPass)
                .build(outPipeline, backend->logicalDevice);
        }

        void VulkanLSceneLine::initOutlineGraphicsPipeline()
        {
            VulkanBackend* backend = getBackend<VulkanBackend>();

            vk::PushConstantRange push;
            push.stageFlags = vk::ShaderStageFlagBits::eVertex | vk::ShaderStageFlagBits::eFragment;
            push.offset     = 0;
            push.size       = sizeof(float) * 8;

            vk::PipelineRasterizationStateCreateInfo rasterization;
            rasterization.depthClampEnable        = VK_FALSE;
            rasterization.rasterizerDiscardEnable = VK_FALSE;
            rasterization.lineWidth               = 1.0f;
            rasterization.depthBiasEnable         = VK_FALSE;
            rasterization.polygonMode             = vk::PolygonMode::eFill;
            rasterization.cullMode                = vk::CullModeFlagBits::eNone;
            rasterization.frontFace               = vk::FrontFace::eCounterClockwise;

            vk::StencilOpState stencilWrite;
            stencilWrite.failOp      = vk::StencilOp::eKeep;
            stencilWrite.passOp      = vk::StencilOp::eReplace;
            stencilWrite.depthFailOp = vk::StencilOp::eKeep;
            stencilWrite.compareOp   = vk::CompareOp::eAlways;
            stencilWrite.compareMask = 0xFF;
            stencilWrite.writeMask   = 0xFF;
            stencilWrite.reference   = 1;

            vk::PipelineDepthStencilStateCreateInfo maskDepth;
            maskDepth.depthBoundsTestEnable = VK_FALSE;
            maskDepth.stencilTestEnable     = VK_TRUE;
            maskDepth.depthWriteEnable      = VK_FALSE;
            maskDepth.depthTestEnable       = VK_TRUE;
            maskDepth.depthCompareOp        = vk::CompareOp::eLessOrEqual;
            maskDepth.minDepthBounds        = 0.0f;
            maskDepth.maxDepthBounds        = 1.0f;
            maskDepth.front                 = stencilWrite;
            maskDepth.back                  = stencilWrite;

            vk::PipelineColorBlendAttachmentState maskBlend = VulkanGraphicsPipeline::createBlendAttachmentState(false);
            maskBlend.colorWriteMask                        = {};

            VulkanShaderStageCreateInfo maskVertex;
            maskVertex.path = "Assets/Engine/Shaders/Vulkan/Scene/Outline.vvert";
            maskVertex.type = vk::ShaderStageFlagBits::eVertex;

            VulkanShaderStageCreateInfo maskFragment;
            maskFragment.path = "Assets/Engine/Shaders/Vulkan/Scene/Outline.vfrag";
            maskFragment.type = vk::ShaderStageFlagBits::eFragment;

            VulkanGraphicsPipelineBuilder()
                .addVertexBinding(VulkanVertex::getBindingDescription())
                .addVertexAttributes(VulkanVertex::getAttributeDescriptions())
                .setInputAssembly(VulkanGraphicsPipeline::createInputAssemblyState())
                .addViewport(backend->getVkViewport(this))
                .addDynamicState(vk::DynamicState::eViewport)
                .addScissor(backend->getVkScissor(this))
                .addDynamicState(vk::DynamicState::eScissor)
                .addShaderStage(maskVertex, backend->logicalDevice)
                .addShaderStage(maskFragment, backend->logicalDevice)
                .addColorBlendingAttachment(maskBlend)
                .setDepthStencil(maskDepth)
                .addDescriptorSetLayout(m_frameDescriptor.setLayout)
                .addPushConstant(push)
                .setRasterization(rasterization)
                .setRenderPass(m_meshPipeline.renderPass)
                .build(m_outlineMaskPipeline, backend->logicalDevice);

            vk::StencilOpState stencilTest;
            stencilTest.failOp      = vk::StencilOp::eKeep;
            stencilTest.passOp      = vk::StencilOp::eKeep;
            stencilTest.depthFailOp = vk::StencilOp::eKeep;
            stencilTest.compareOp   = vk::CompareOp::eNotEqual;
            stencilTest.compareMask = 0xFF;
            stencilTest.writeMask   = 0x00;
            stencilTest.reference   = 1;

            vk::PipelineDepthStencilStateCreateInfo ringDepth = maskDepth;
            ringDepth.front                                   = stencilTest;
            ringDepth.back                                    = stencilTest;

            VulkanShaderStageCreateInfo ringVertex;
            ringVertex.path = "Assets/Engine/Shaders/Vulkan/Scene/Outline.vvert";
            ringVertex.type = vk::ShaderStageFlagBits::eVertex;

            VulkanShaderStageCreateInfo ringFragment;
            ringFragment.path = "Assets/Engine/Shaders/Vulkan/Scene/Outline.vfrag";
            ringFragment.type = vk::ShaderStageFlagBits::eFragment;

            VulkanGraphicsPipelineBuilder()
                .addVertexBinding(VulkanVertex::getBindingDescription())
                .addVertexAttributes(VulkanVertex::getAttributeDescriptions())
                .setInputAssembly(VulkanGraphicsPipeline::createInputAssemblyState())
                .addViewport(backend->getVkViewport(this))
                .addDynamicState(vk::DynamicState::eViewport)
                .addScissor(backend->getVkScissor(this))
                .addDynamicState(vk::DynamicState::eScissor)
                .addShaderStage(ringVertex, backend->logicalDevice)
                .addShaderStage(ringFragment, backend->logicalDevice)
                .addColorBlendingAttachment(VulkanGraphicsPipeline::createBlendAttachmentState(false))
                .setDepthStencil(ringDepth)
                .addDescriptorSetLayout(m_frameDescriptor.setLayout)
                .addPushConstant(push)
                .setRasterization(rasterization)
                .setRenderPass(m_meshPipeline.renderPass)
                .build(m_outlinePipeline, backend->logicalDevice);
        }

        void VulkanLSceneLine::initFramebuffers()
        {
            VulkanBackend* backend  = getBackend<VulkanBackend>();
            vk::Viewport   viewport = backend->getVkViewport(this);

            for (VulkanSwapchainImage& frame : backend->swapchain.images)
            {
                VulkanFrameBufferCreateInfo createInfo;
                createInfo.id            = m_id;
                createInfo.logicalDevice = backend->logicalDevice;
                createInfo.renderPass    = m_meshPipeline.renderPass;
                createInfo.extent.width  = viewport.width;
                createInfo.extent.height = viewport.height;
                createInfo.attachments.push_back(frame.targetImage.view);
                createInfo.attachments.push_back(frame.depthImage.view);

                frame.addBuffer(createInfo);
            }
        }

        void VulkanLSceneLine::drawOutlineMeshes(
            const Frame&       inFrame,
            vk::CommandBuffer  inCommandBuffer,
            vk::PipelineLayout inLayout,
            float              inOffsetX,
            float              inOffsetY
        ) const
        {
            const float push[8] = {
                inOffsetX,
                inOffsetY,
                0.0f,
                0.0f,
                static_cast<float>(OUTLINE_COLOR.r) / 255.0f,
                static_cast<float>(OUTLINE_COLOR.g) / 255.0f,
                static_cast<float>(OUTLINE_COLOR.b) / 255.0f,
                static_cast<float>(OUTLINE_COLOR.a) / 255.0f
            };
            inCommandBuffer.pushConstants(
                inLayout,
                vk::ShaderStageFlagBits::eVertex | vk::ShaderStageFlagBits::eFragment,
                0,
                sizeof(push),
                push
            );

            for (const DrawPoly& draw : inFrame.getOutlineDraws())
            {
                inCommandBuffer.drawIndexed(
                    draw.indexCount,
                    draw.instanceCount,
                    draw.indexStart,
                    draw.vertexStart,
                    draw.instanceStart
                );
            }
        }

        VulkanLSceneLineImmediateBuffer& VulkanLSceneLine::immediateBufferFor(VulkanFrame& inFrame)
        {
            VulkanBackend* backend = getBackend<VulkanBackend>();
            if (m_immediateBuffers.size() != backend->frames.size())
            {
                for (VulkanLSceneLineImmediateBuffer& buffer : m_immediateBuffers)
                {
                    destroyImmediateBuffer(buffer);
                }

                m_immediateBuffers.resize(backend->frames.size());
            }

            if (m_immediateBuffers.empty())
            {
                m_immediateBuffers.resize(1);
            }

            const std::ptrdiff_t index = &inFrame - backend->frames.data();
            if (index < 0 || static_cast<std::size_t>(index) >= m_immediateBuffers.size())
            {
                return m_immediateBuffers.front();
            }

            return m_immediateBuffers[static_cast<std::size_t>(index)];
        }

        void VulkanLSceneLine::ensureImmediateVertexBuffer(
            VulkanLSceneLineImmediateBuffer& outBuffer, std::size_t inBytes
        )
        {
            const std::size_t required = std::max<std::size_t>(inBytes, 1);
            if (outBuffer.vertexBuffer.instance && required <= outBuffer.vertexCapacity)
            {
                return;
            }

            VulkanBackend* backend = getBackend<VulkanBackend>();
            if (outBuffer.vertexBuffer.instance)
            {
                if (outBuffer.vertexMapped)
                {
                    backend->logicalDevice.unmapMemory(outBuffer.vertexBuffer.memory);
                    outBuffer.vertexMapped = nullptr;
                }

                outBuffer.vertexBuffer.destroy(backend->logicalDevice);
                outBuffer.vertexCapacity = 0;
            }

            VulkanBufferCreateInfo createInfo;
            createInfo.physicalDevice = backend->physicalDevice;
            createInfo.logicalDevice  = backend->logicalDevice;
            createInfo.size           = required;
            createInfo.usage          = vk::BufferUsageFlagBits::eVertexBuffer;
            createInfo.memoryProperties =
                vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent;

            outBuffer.vertexBuffer.init(createInfo);
            outBuffer.vertexMapped   = backend->logicalDevice.mapMemory(outBuffer.vertexBuffer.memory, 0, required);
            outBuffer.vertexCapacity = required;
        }

        void VulkanLSceneLine::ensureImmediateIndexBuffer(
            VulkanLSceneLineImmediateBuffer& outBuffer, std::size_t inBytes
        )
        {
            if (inBytes == 0)
            {
                return;
            }

            const std::size_t required = inBytes;
            if (outBuffer.indexBuffer.instance && required <= outBuffer.indexCapacity)
            {
                return;
            }

            VulkanBackend* backend = getBackend<VulkanBackend>();
            if (outBuffer.indexBuffer.instance)
            {
                if (outBuffer.indexMapped)
                {
                    backend->logicalDevice.unmapMemory(outBuffer.indexBuffer.memory);
                    outBuffer.indexMapped = nullptr;
                }

                outBuffer.indexBuffer.destroy(backend->logicalDevice);
                outBuffer.indexCapacity = 0;
            }

            VulkanBufferCreateInfo createInfo;
            createInfo.physicalDevice = backend->physicalDevice;
            createInfo.logicalDevice  = backend->logicalDevice;
            createInfo.size           = required;
            createInfo.usage          = vk::BufferUsageFlagBits::eIndexBuffer;
            createInfo.memoryProperties =
                vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent;

            outBuffer.indexBuffer.init(createInfo);
            outBuffer.indexMapped   = backend->logicalDevice.mapMemory(outBuffer.indexBuffer.memory, 0, required);
            outBuffer.indexCapacity = required;
        }

        void VulkanLSceneLine::destroyImmediateBuffer(VulkanLSceneLineImmediateBuffer& inBuffer)
        {
            VulkanBackend* backend = getBackend<VulkanBackend>();

            if (inBuffer.vertexBuffer.instance)
            {
                if (inBuffer.vertexMapped)
                {
                    backend->logicalDevice.unmapMemory(inBuffer.vertexBuffer.memory);
                    inBuffer.vertexMapped = nullptr;
                }

                inBuffer.vertexBuffer.destroy(backend->logicalDevice);
                inBuffer.vertexCapacity = 0;
            }

            if (inBuffer.indexBuffer.instance)
            {
                if (inBuffer.indexMapped)
                {
                    backend->logicalDevice.unmapMemory(inBuffer.indexBuffer.memory);
                    inBuffer.indexMapped = nullptr;
                }

                inBuffer.indexBuffer.destroy(backend->logicalDevice);
                inBuffer.indexCapacity = 0;
            }
        }

        void VulkanLSceneLine::destroyImmediateBuffers()
        {
            for (VulkanLSceneLineImmediateBuffer& buffer : m_immediateBuffers)
            {
                destroyImmediateBuffer(buffer);
            }

            m_immediateBuffers.clear();
        }

        void VulkanLSceneLine::uploadImmediateGeometry(VulkanLSceneLineImmediateBuffer& outBuffer, const Frame& inFrame)
        {
            const Vertex::List&    vertices = inFrame.getImmediateVertices();
            const Vertex::Indices& indices  = inFrame.getImmediateIndices();

            ensureImmediateVertexBuffer(outBuffer, sizeof(Vertex) * vertices.size());
            if (outBuffer.vertexMapped && !vertices.empty())
            {
                std::memcpy(outBuffer.vertexMapped, vertices.data(), sizeof(Vertex) * vertices.size());
            }

            ensureImmediateIndexBuffer(outBuffer, sizeof(Vertex::Index) * indices.size());
            if (outBuffer.indexMapped && !indices.empty())
            {
                std::memcpy(outBuffer.indexMapped, indices.data(), sizeof(Vertex::Index) * indices.size());
            }
        }

        void VulkanLSceneLine::drawLineList(
            vk::CommandBuffer inCommandBuffer, const DrawPoly& inDraw, vk::Buffer inIndexBuffer
        ) const
        {
            if (inDraw.indexCount == 0)
            {
                inCommandBuffer
                    .draw(inDraw.vertexCount, inDraw.instanceCount, inDraw.vertexStart, inDraw.instanceStart);

                return;
            }

            if (!inIndexBuffer)
            {
                return;
            }

            inCommandBuffer.bindIndexBuffer(inIndexBuffer, 0, vk::IndexType::eUint32);
            inCommandBuffer.drawIndexed(
                inDraw.indexCount,
                inDraw.instanceCount,
                inDraw.indexStart,
                inDraw.vertexStart,
                inDraw.instanceStart
            );
        }
    }
}
