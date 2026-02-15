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
            static constexpr inline const char* ID = "Engine_Scene_Shadow";

        public:
            VulkanLSceneShadow();

        protected:
            bool onInit() override;
            bool onDestroy() override;
            bool onRebuild() override;
            bool onSetup(const Frame& inFrame) override;
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