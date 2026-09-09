#include "Chicane/Renderer/Backend/Vulkan/Layer/Scene/Line.hpp"

#include <cstring>

#include "Chicane/Renderer/Backend/Vulkan.hpp"
#include "Chicane/Renderer/Backend/Vulkan/Descriptor/Pool.hpp"
#include "Chicane/Renderer/Backend/Vulkan/Descriptor/Pool/CreateInfo.hpp"
#include "Chicane/Renderer/Backend/Vulkan/Descriptor/SetLayout.hpp"
#include "Chicane/Renderer/Backend/Vulkan/Descriptor/SetLayout/BidingsCreateInfo.hpp"
#include "Chicane/Renderer/Backend/Vulkan/Frame.hpp"
#include "Chicane/Renderer/Backend/Vulkan/GraphicsPipeline/Builder.hpp"
#include "Chicane/Renderer/Backend/Vulkan/Layer/Scene.hpp"
#include "Chicane/Renderer/Backend/Vulkan/Vertex.hpp"
#include "Chicane/Renderer/Debug/Mode.hpp"
#include "Chicane/Renderer/Instance.hpp"

namespace Chicane
{
    namespace Renderer
    {
        VulkanLSceneLine::VulkanLSceneLine()
            : Layer(SCENE_LINE_LAYER_ID),
              m_overlayBufferCapacity(0),
              m_overlayVertexCount(0),
              m_clear({vk::ClearColorValue(0.0f, 0.0f, 0.0f, 0.0f), vk::ClearDepthStencilValue(1.0f, 0)})
        {}

        void VulkanLSceneLine::onInit()
        {
            initFrameResources();
            initMeshGraphicsPipeline();
            initOverlayGraphicsPipeline();
            initOutlineGraphicsPipeline();
            initFramebuffers();
        }

        void VulkanLSceneLine::onRestart()
        {
            initFramebuffers();
        }

        void VulkanLSceneLine::onDestruction()
        {
            destroyOverlayBuffer();
            destroyFrameResources();

            getBackend<VulkanBackend>()->logicalDevice.waitIdle();

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

            if (m_overlayPipeline.instance)
            {
                getBackend<VulkanBackend>()->logicalDevice.destroyPipeline(m_overlayPipeline.instance);
                getBackend<VulkanBackend>()->logicalDevice.destroyPipelineLayout(m_overlayPipeline.layout);
                m_overlayPipeline.instance   = nullptr;
                m_overlayPipeline.layout     = nullptr;
                m_overlayPipeline.renderPass = nullptr;
            }

            m_meshPipeline.destroy();
        }

        bool VulkanLSceneLine::shouldDrawMeshWireframe(const Frame& inFrame) const
        {
            if (inFrame.hasDraws(DrawPolyType::e3D, DrawPolyMode::Line))
            {
                return true;
            }

            const Instance* renderer = getBackend()->getRenderer();

            return renderer->hasDebug(DebugMode::Meshes) && inFrame.hasDraws(DrawPolyType::e3D, DrawPolyMode::Fill);
        }

        bool VulkanLSceneLine::shouldDrawOverlay() const
        {
            const Instance* renderer = getBackend()->getRenderer();

            return (renderer->hasDebug(DebugMode::Bounds) || renderer->hasDebug(DebugMode::Traces) ||
                    renderer->hasDebug(DebugMode::Colliders)) &&
                   renderer->hasDebugOverlay();
        }

        bool VulkanLSceneLine::shouldDrawOutline(const Frame& inFrame) const
        {
            return inFrame.hasOutlineDraws();
        }

        bool VulkanLSceneLine::onBeginRender(const Frame& inFrame)
        {
            return shouldDrawMeshWireframe(inFrame) || shouldDrawOverlay() || shouldDrawOutline(inFrame);
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

                vk::Buffer     vertexBuffers[] = {parent->modelVertexBuffer.instance};
                vk::DeviceSize offsets[]       = {0};
                commandBuffer.bindVertexBuffers(0, 1, vertexBuffers, offsets);
                commandBuffer.bindIndexBuffer(parent->modelIndexBuffer.instance, 0, vk::IndexType::eUint32);

                auto drawBatch = [&](DrawPolyMode inMode)
                {
                    for (const DrawPoly& draw : inFrame.getDraws(DrawPolyType::e3D, inMode))
                    {
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
                    drawBatch(DrawPolyMode::Fill);
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

            if (shouldDrawOverlay())
            {
                Vertex::List vertices = renderer->getDebugOverlayVertices();
                if (!vertices.empty())
                {
                    uploadOverlayBuffer(vertices);

                    m_overlayPipeline.bind(commandBuffer);
                    m_overlayPipeline.bind(commandBuffer, 0, frame.getDescriptorSet(m_id));

                    vk::Buffer     vertexBuffers[] = {m_overlayBuffer.instance};
                    vk::DeviceSize offsets[]       = {0};
                    commandBuffer.bindVertexBuffers(0, 1, vertexBuffers, offsets);
                    commandBuffer.draw(m_overlayVertexCount, 1, 0, 0);
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

        void VulkanLSceneLine::initOverlayGraphicsPipeline()
        {
            VulkanBackend* backend = getBackend<VulkanBackend>();

            VulkanShaderStageCreateInfo vertexShader;
            vertexShader.path = "Assets/Engine/Shaders/Vulkan/Scene/Line/Overlay.vvert";
            vertexShader.type = vk::ShaderStageFlagBits::eVertex;

            VulkanShaderStageCreateInfo fragmentShader;
            fragmentShader.path = "Assets/Engine/Shaders/Vulkan/Scene/Line/Overlay.vfrag";
            fragmentShader.type = vk::ShaderStageFlagBits::eFragment;

            vk::PipelineDepthStencilStateCreateInfo depth;
            depth.depthBoundsTestEnable = VK_FALSE;
            depth.stencilTestEnable     = VK_FALSE;
            depth.depthWriteEnable      = VK_TRUE;
            depth.depthTestEnable       = VK_TRUE;
            depth.depthCompareOp        = vk::CompareOp::eLessOrEqual;
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
                .build(m_overlayPipeline, backend->logicalDevice);
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

        void VulkanLSceneLine::ensureOverlayBuffer(std::size_t inVertexCount)
        {
            const std::size_t required = sizeof(Vertex) * std::max<std::size_t>(inVertexCount, 1);
            if (m_overlayBuffer.instance && required <= m_overlayBufferCapacity)
            {
                return;
            }

            destroyOverlayBuffer();

            VulkanBackend* backend = getBackend<VulkanBackend>();

            VulkanBufferCreateInfo createInfo;
            createInfo.physicalDevice = backend->physicalDevice;
            createInfo.logicalDevice  = backend->logicalDevice;
            createInfo.size           = required;
            createInfo.usage          = vk::BufferUsageFlagBits::eVertexBuffer;
            createInfo.memoryProperties =
                vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent;

            m_overlayBuffer.init(createInfo);
            m_overlayBufferCapacity = required;
        }

        void VulkanLSceneLine::destroyOverlayBuffer()
        {
            if (!m_overlayBuffer.instance)
            {
                return;
            }

            VulkanBackend* backend = getBackend<VulkanBackend>();
            m_overlayBuffer.destroy(backend->logicalDevice);
            m_overlayBufferCapacity = 0;
            m_overlayVertexCount    = 0;
        }

        void VulkanLSceneLine::uploadOverlayBuffer(const Vertex::List& inVertices)
        {
            ensureOverlayBuffer(inVertices.size());

            VulkanBackend*       backend = getBackend<VulkanBackend>();
            const vk::DeviceSize size    = sizeof(Vertex) * inVertices.size();

            void* writeLocation = backend->logicalDevice.mapMemory(m_overlayBuffer.memory, 0, size);
            memcpy(writeLocation, inVertices.data(), size);
            backend->logicalDevice.unmapMemory(m_overlayBuffer.memory);

            m_overlayVertexCount = static_cast<std::uint32_t>(inVertices.size());
        }
    }
}
