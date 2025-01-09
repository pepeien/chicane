#pragma once

#include "Chicane/Base.hpp"
#include "Chicane/Core/Window.hpp"
#include "Chicane/Game/Level/Instance.hpp"
#include "Chicane/Renderer/CubeMap/Manager.hpp"
#include "Chicane/Renderer/Layer.hpp"
#include "Chicane/Renderer/Vulkan.hpp"

namespace Chicane
{
    namespace Vulkan
    {
        class SkyboxLayer : public Layer::Instance
        {
        public:
            SkyboxLayer();
            ~SkyboxLayer();

        public:
            void build() override;
            void destroy() override;
            void rebuild() override;

            void render(void* outData) override;

        private:
            void loadEvents();

            void initFrameDescriptorSetLayout();
            void initMaterialDescriptorSetLayout();
            void initGraphicsPipeline();
            void initFramebuffers();
            void initFrameResources();
            void initMaterialResources();

            void buildCubeMap();
            void renderCubeMap(const vk::CommandBuffer& inCommandBuffer);

        private:
            Vulkan::Renderer::Internals                         m_internals;

            std::unique_ptr<Vulkan::GraphicsPipeline::Instance> m_graphicsPipeline;

            Vulkan::Descriptor::Bundle                          m_frameDescriptor;
            Vulkan::Descriptor::Bundle                          m_materialDescriptor;

            std::unique_ptr<CubeMap::Instance>                  m_cubeMap;
            Chicane::CubeMap::Manager*                          m_cubeMapManager;

            std::vector<vk::ClearValue>                         m_clearValues;
        };
    }
}