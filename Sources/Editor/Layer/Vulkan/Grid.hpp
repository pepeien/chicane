#pragma once

#include <vulkan/vulkan.hpp>

#include <Chicane/Renderer/Backend/Vulkan/Descriptor/Bundle.hpp>
#include <Chicane/Renderer/Backend/Vulkan/GraphicsPipeline.hpp>
#include <Chicane/Renderer/Layer.hpp>

namespace Editor
{
    class VulkanLGrid : public Chicane::Renderer::Layer<>
    {
    public:
        VulkanLGrid();

    protected:
        bool onInit() override;
        bool onDestroy() override;
        bool onRebuild() override;
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