#pragma once

#include "Chicane/Base.hpp"
#include "Chicane/Core/Window.hpp"
#include "Chicane/Game/Level.hpp"
#include "Chicane/Grid/View.hpp"
#include "Chicane/Renderer/Layer.hpp"
#include "Chicane/Renderer/Vulkan.hpp"

namespace Chicane
{
    namespace Vulkan
    {
        class UILayer : public Layer
        {
        public:
            UILayer();
            ~UILayer();

        public:
            void build() override;
            void destroy() override;
            void rebuild() override;

            void onEvent(const SDL_Event& inEvent) override;

            void setup(void* outData) override;
            void render(void* outData) override;

        private:
            void loadEvents();

            void initDescriptorPool();
            void initRenderpass();
            void initFramebuffers();
            void setupImgui();
            void initImgui();

        private:
            // Window
            Window*                     m_window;

            // UI
            Grid::View*                 m_view;

            // Render
            Vulkan::Renderer::Internals m_internals;
            vk::RenderPass              m_renderPass;
            vk::DescriptorPool          m_descriptorPool;
            std::vector<vk::ClearValue> m_clearValues;
        };
    }
}