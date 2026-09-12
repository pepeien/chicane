#pragma once

#include "Chicane/Renderer.hpp"
#include "Chicane/Renderer/Backend/Vulkan/Buffer.hpp"
#include "Chicane/Renderer/Backend/Vulkan/Descriptor/Bundle.hpp"
#include "Chicane/Renderer/Backend/Vulkan/GraphicsPipeline.hpp"
#include "Chicane/Renderer/Layer.hpp"

namespace Chicane
{
    namespace Renderer
    {
        class CHICANE_RENDERER VulkanLSceneShadow : public Layer
        {
        public:
            VulkanLSceneShadow();

        protected:
            void onInit() override;
            void onRestart() override;
            void onDestruction() override;

            bool onBeginRender(const Frame& inFrame) override;
            void onRender(const Frame& inFrame, void* inData = nullptr) override;

        private:
            // Resource
            void initFrameResources();
            void destroyFrameResources();

            // Pipeline
            void initGraphicsPipeline();
            void initFramebuffers();

        private:
            VulkanGraphicsPipeline      m_graphicsPipeline;

            VulkanDescriptorBundle      m_frameDescriptor;

            std::vector<vk::ClearValue> m_clear;
        };
    }
}