#include "Chicane/Core/Layers/Skybox.hpp"

#include "Chicane/Core/Log.hpp"
#include "Chicane/Core/Window.hpp"
#include "Chicane/Game/State.hpp"

namespace Chicane
{
    SkyboxLayer::SkyboxLayer(Window* inWindow)
        : Layer("Skybox"),
        m_renderer(inWindow->getRenderer()),
        m_manager(std::make_unique<CubeMap::Manager>())
    {
        if (State::hasLevel() == false)
        {
            return;
        }

        m_level         = State::getLevel();
        m_isInitialized = m_level->hasSkybox();
    }

    SkyboxLayer::~SkyboxLayer()
    {
        m_renderer->m_logicalDevice.waitIdle();

        // Graphics Pipeline
        m_graphicsPipeline.reset();

        // Descriptors
        m_renderer->m_logicalDevice.destroyDescriptorSetLayout(
            m_frameDescriptor.setLayout
        );

        m_renderer->m_logicalDevice.destroyDescriptorSetLayout(
            m_materialDescriptor.setLayout
        );
        m_renderer->m_logicalDevice.destroyDescriptorPool(
            m_materialDescriptor.pool
        );

        // Managers
        m_manager.reset();
    }

    void SkyboxLayer::build()
    {
        if (!m_isInitialized)
        {
            return;
        }

        loadAssets();
        initFrameDescriptorSetLayout();
        initMaterialDescriptorSetLayout();
        initGraphicsPipeline();
        initFramebuffers();
        initFrameResources();
        initMaterialResources();
        buildAssets();
    }

    void SkyboxLayer::destroy()
    {
        if (!m_isInitialized)
        {
            return;
        }

        m_renderer->m_logicalDevice.destroyDescriptorPool(
            m_frameDescriptor.pool
        );
    }

    void SkyboxLayer::rebuild()
    {
        if (!m_isInitialized)
        {
            return;
        }

        initFramebuffers();
        initFrameResources();
    }

    void SkyboxLayer::render(
        Frame::Instance& outFrame,
        const vk::CommandBuffer& inCommandBuffer,
        const vk::Extent2D& inSwapChainExtent
    )
    {
        if (!m_isInitialized)
        {
            return;
        }

        // Renderpass
        std::vector<vk::ClearValue> clearValues;
        clearValues.push_back(vk::ClearColorValue(0.0f, 0.0f, 0.0f, 0.0f));

        vk::RenderPassBeginInfo renderPassBeginInfo = {};
        renderPassBeginInfo.renderPass          = m_graphicsPipeline->renderPass;
        renderPassBeginInfo.framebuffer         = outFrame.getFramebuffer(m_id);
        renderPassBeginInfo.renderArea.offset.x = 0;
        renderPassBeginInfo.renderArea.offset.y = 0;
        renderPassBeginInfo.renderArea.extent   = inSwapChainExtent;
        renderPassBeginInfo.clearValueCount     = static_cast<uint32_t>(clearValues.size());
        renderPassBeginInfo.pClearValues        = clearValues.data();

        inCommandBuffer.beginRenderPass(
            &renderPassBeginInfo,
            vk::SubpassContents::eInline
        );

        // Viewport
        m_renderer->updateViewport(inCommandBuffer);

        // Preparing
        inCommandBuffer.bindPipeline(
            vk::PipelineBindPoint::eGraphics,
            m_graphicsPipeline->instance
        );

        inCommandBuffer.bindDescriptorSets(
            vk::PipelineBindPoint::eGraphics,
            m_graphicsPipeline->layout,
            0,
            outFrame.getDescriptorSet(m_id),
            nullptr
        );

        m_manager->bind(
            "Skybox",
            inCommandBuffer,
            m_graphicsPipeline->layout
        );

        // Drawing
        m_manager->draw(
            "Skybox",
            inCommandBuffer
        );

        inCommandBuffer.endRenderPass();

        return;
    }

    void SkyboxLayer::loadAssets()
    {
        if (!m_isInitialized)
        {
            return;
        }

        Box::Instance cubemap = m_level->getSkybox();

        if (cubemap.entries.size() < 6)
        {
            throw std::runtime_error("The level cubemap isn't valid");
        }

        m_manager->add(
            "Skybox",
            {
                cubemap.entries[0].data, // Positive X
                cubemap.entries[1].data, // Negative X
                cubemap.entries[2].data, // Positive Y
                cubemap.entries[3].data, // Negative Y
                cubemap.entries[4].data, // Positive Z
                cubemap.entries[5].data  // Negative Z
            }
        );
    }

    void SkyboxLayer::initFrameDescriptorSetLayout()
    {
        Descriptor::SetLayoutBidingsCreateInfo frameLayoutBidings;
        frameLayoutBidings.count = 1;

        /// Camera
        frameLayoutBidings.indices.push_back(0);
        frameLayoutBidings.types.push_back(vk::DescriptorType::eUniformBuffer);
        frameLayoutBidings.counts.push_back(1);
        frameLayoutBidings.stages.push_back(vk::ShaderStageFlagBits::eVertex);

        Descriptor::initSetLayout(
            m_frameDescriptor.setLayout,
            m_renderer->m_logicalDevice,
            frameLayoutBidings
        );
    }

    void SkyboxLayer::initMaterialDescriptorSetLayout()
    {
        Descriptor::SetLayoutBidingsCreateInfo materialLayoutBidings;
        materialLayoutBidings.count = 1;
        materialLayoutBidings.indices.push_back(0);
        materialLayoutBidings.types.push_back(vk::DescriptorType::eCombinedImageSampler);
        materialLayoutBidings.counts.push_back(1);
        materialLayoutBidings.stages.push_back(vk::ShaderStageFlagBits::eFragment);

        Descriptor::initSetLayout(
            m_materialDescriptor.setLayout,
            m_renderer->m_logicalDevice,
            materialLayoutBidings
        );
    }

    void SkyboxLayer::initGraphicsPipeline()
    {
        GraphicsPipeline::CreateInfo graphicsPipelineCreateInfo = {};
        graphicsPipelineCreateInfo.canOverwrite         = false;
        graphicsPipelineCreateInfo.hasVertices          = false;
        graphicsPipelineCreateInfo.hasDepth             = false;
        graphicsPipelineCreateInfo.logicalDevice        = m_renderer->m_logicalDevice;
        graphicsPipelineCreateInfo.vertexShaderPath     = "Content/Chicane/Shaders/sky.vert.spv";
        graphicsPipelineCreateInfo.fragmentShaderPath   = "Content/Chicane/Shaders/sky.frag.spv";
        graphicsPipelineCreateInfo.swapChainExtent      = m_renderer->m_swapChain.extent;
        graphicsPipelineCreateInfo.swapChainImageFormat = m_renderer->m_swapChain.format;
        graphicsPipelineCreateInfo.descriptorSetLayouts = { m_frameDescriptor.setLayout, m_materialDescriptor.setLayout };

        m_graphicsPipeline = std::make_unique<GraphicsPipeline::Instance>(graphicsPipelineCreateInfo);
    }

    void SkyboxLayer::initFramebuffers()
    {
        for (Frame::Instance& frame : m_renderer->m_swapChain.images)
        {
            Frame::Buffer::CreateInfo framebufferCreateInfo = {};
            framebufferCreateInfo.id              = m_id;
            framebufferCreateInfo.logicalDevice   = m_renderer->m_logicalDevice;
            framebufferCreateInfo.renderPass      = m_graphicsPipeline->renderPass;
            framebufferCreateInfo.swapChainExtent = m_renderer->m_swapChain.extent;
            framebufferCreateInfo.attachments.push_back(frame.imageView);

            Frame::Buffer::init(frame, framebufferCreateInfo);
        }
    }

    void SkyboxLayer::initFrameResources()
    {
        Descriptor::PoolCreateInfo frameDescriptorPoolCreateInfo;
        frameDescriptorPoolCreateInfo.size = static_cast<uint32_t>(m_renderer->m_swapChain.images.size());
        frameDescriptorPoolCreateInfo.types.push_back(vk::DescriptorType::eUniformBuffer);

        Descriptor::initPool(
            m_frameDescriptor.pool,
            m_renderer->m_logicalDevice,
            frameDescriptorPoolCreateInfo
        );

        for (Frame::Instance& frame : m_renderer->m_swapChain.images)
        {
            vk::DescriptorSet descriptorSet;
            Descriptor::initSet(
                descriptorSet,
                m_renderer->m_logicalDevice,
                m_frameDescriptor.setLayout,
                m_frameDescriptor.pool
            );
            frame.addDescriptorSet(m_id, descriptorSet);

            vk::WriteDescriptorSet writeDescriptorSet;
            writeDescriptorSet.dstSet          = descriptorSet;
            writeDescriptorSet.dstBinding      = 0;
            writeDescriptorSet.dstArrayElement = 0;
            writeDescriptorSet.descriptorCount = 1;
            writeDescriptorSet.descriptorType  = vk::DescriptorType::eUniformBuffer;
            writeDescriptorSet.pBufferInfo     = &frame.cameraDescriptorBufferInfo;
            frame.addWriteDescriptorSet(writeDescriptorSet);
        }
    }

    void SkyboxLayer::initMaterialResources()
    {
        Descriptor::PoolCreateInfo materialDescriptorPoolCreateInfo;
        materialDescriptorPoolCreateInfo.size  = 1;
        materialDescriptorPoolCreateInfo.types.push_back(vk::DescriptorType::eCombinedImageSampler);

        Descriptor::initPool(
            m_materialDescriptor.pool,
            m_renderer->m_logicalDevice,
            materialDescriptorPoolCreateInfo
        );
    }

    void SkyboxLayer::buildAssets()
    {
        m_manager->build(
            m_renderer->m_logicalDevice,
            m_renderer->m_physicalDevice,
            m_renderer->m_mainCommandBuffer,
            m_renderer->m_graphicsQueue,
            m_materialDescriptor.setLayout,
            m_materialDescriptor.pool
        );
    }
}