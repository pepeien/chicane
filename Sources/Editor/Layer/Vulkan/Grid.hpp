#pragma once

#include <vulkan/vulkan.hpp>

#include <Chicane/Renderer/Backend/Vulkan/Descriptor/Bundle.hpp>
#include <Chicane/Renderer/Backend/Vulkan/GraphicsPipeline.hpp>
#include <Chicane/Renderer/Frame.hpp>
#include <Chicane/Renderer/Layer.hpp>

namespace Editor
{
    class VulkanLUI : public Chicane::Renderer::Layer
    {
    public:
        VulkanLUI();

    protected:
        void onInit() override;
        void onRestart() override;
        void onDestruction() override;

        void onRender(const Chicane::Renderer::Frame& inFrame, void* inData = nullptr) override;

    private:
        // Resource
        void initFrameResources();
        void destroyFrameResources();

        // Pipeline
        void initGraphicsPipeline();
        void initFramebuffers();

        // Viewport
        void initViewport();

    private:
        Chicane::Renderer::VulkanGraphicsPipeline m_graphicsPipeline;

        Chicane::Renderer::VulkanDescriptorBundle m_frameDescriptor;

        std::vector<vk::ClearValue> m_clear;
    };
}