#pragma once

#include <vulkan/vulkan.hpp>

#include <Chicane/Renderer/Backend/Vulkan/Descriptor/Bundle.hpp>
#include <Chicane/Renderer/Backend/Vulkan/GraphicsPipeline.hpp>
#include <Chicane/Renderer/Layer.hpp>

namespace Editor
{
    class VulkanLGrid : public Chicane::Renderer::Layer
    {
    public:
        VulkanLGrid();

    protected:
        void onInit() override;
        void onShutdown() override;
        void onRestart() override;

        void onResize(const Chicane::Vec<2, std::uint32_t>& inResolution) override;

        void onRender(const Chicane::Renderer::Frame& inFrame, void* inData = nullptr) override;

    private:
        // Resource
        void initFrameResources();
        void destroyFrameResources();

        // Pipeline
        void initGraphicsPipeline();
        void initFramebuffers();

    private:
        Chicane::Renderer::VulkanGraphicsPipeline m_graphicsPipeline;

        Chicane::Renderer::VulkanDescriptorBundle m_frameDescriptor;

        std::vector<vk::ClearValue> m_clear;
    };
}