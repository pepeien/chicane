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
            VulkanLSceneSky();
            ~VulkanLSceneSky();

        public:
            bool onInit() override;
            bool onDestroy() override;
            bool onRebuild() override;
            void onLoad(const DrawSky& inResource) override;
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
            std::unique_ptr<VulkanGraphicsPipeline> m_graphicsPipeline;

            VulkanDescriptorBundle                  m_textureDescriptor;
            VulkanDescriptorBundle                  m_frameDescriptor;

            std::unique_ptr<VulkanSky>              m_sky;

            std::vector<vk::ClearValue>             m_clear;
        };
    }
}