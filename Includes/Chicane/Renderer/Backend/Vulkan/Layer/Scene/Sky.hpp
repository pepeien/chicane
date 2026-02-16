#pragma once

#include "Chicane/Renderer.hpp"
#include "Chicane/Renderer/Backend/Vulkan/Buffer.hpp"
#include "Chicane/Renderer/Backend/Vulkan/Descriptor/Bundle.hpp"
#include "Chicane/Renderer/Backend/Vulkan/GraphicsPipeline.hpp"
#include "Chicane/Renderer/Backend/Vulkan/Sky.hpp"
#include "Chicane/Renderer/Layer.hpp"

namespace Chicane
{
    namespace Renderer
    {
        class CHICANE_RENDERER VulkanLSceneSky : public Layer
        {
        public:
            static constexpr inline const char* ID = "Engine_Scene_Sky";

        public:
            VulkanLSceneSky();

        public:
            void onInit() override;
            void onShutdown() override;
            void onRestart() override;
            void onDestruction() override;

            void onLoad(const DrawSky& inResource) override;

            bool onBeginRender(const Frame& inFrame) override;
            void onRender(const Frame& inFrame, void* inData) override;

        private:
            // Resource
            void initFrameResources();
            void destroyFrameResources();

            // Pipeline
            void initGraphicsPipeline();
            void initFramebuffers();

            // Texture
            void buildTextureDescriptor();
            void buildTextureData(const DrawSky& inData);
            void destroyTextureData();

        private:
            VulkanGraphicsPipeline      m_graphicsPipeline;

            VulkanDescriptorBundle      m_textureDescriptor;
            VulkanDescriptorBundle      m_frameDescriptor;

            std::unique_ptr<VulkanSky>  m_sky;

            std::vector<vk::ClearValue> m_clear;
        };
    }
}