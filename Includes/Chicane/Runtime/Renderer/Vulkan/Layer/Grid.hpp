#pragma once

#include "Core.hpp"
#include "Core/Window.hpp"
#include "Runtime/Game/Level/Instance.hpp"
#include "Grid/View.hpp"
#include "Runtime/Renderer/Layer.hpp"
#include "Runtime/Renderer/Vulkan.hpp"

namespace Chicane
{
    namespace Vulkan
    {
        class CHICANE LGrid : public Layer::Instance
        {
        public:
            LGrid();
            ~LGrid();

        public:
            void build() override;
            void destroy() override;
            void rebuild() override;
    
            void setup(void* outData) override;
            void render(void* outData) override;

        private:
            void loadEvents();

            void initDescriptorPool();
            void initRenderpass();
            void initFramebuffers();

        private:
            // Window
            Window::Instance*           m_window;

            // UI
            Grid::View*                 m_view;

            // Vulkan
            Renderer::Internals         m_internals;
            vk::RenderPass              m_renderPass;
            vk::DescriptorPool          m_descriptorPool;
            std::vector<vk::ClearValue> m_clearValues;
        };
    }
}