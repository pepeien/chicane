#pragma once

#include "Chicane/Renderer.hpp"
#include "Chicane/Renderer/Layer.hpp"
#include "Chicane/Renderer/Viewport.hpp"

namespace Chicane
{
    namespace Renderer
    {
        class CHICANE_RENDERER OpenGLLSceneShadow : public Layer
        {
        public:
            static constexpr inline const char* ID = "Engine_Scene_Shadow";

        public:
            OpenGLLSceneShadow();

        public:
            void onInit() override;
            void onDestruction() override;

            bool onBeginRender(const Frame& inFrame) override;
            void onRender(const Frame& inFrame, void* inData) override;
            void onEndRender() override;

        private:
            void buildShader();
            void destroyShader();

            void buildShadowMap();
            void destroyShadowMap();

        private:
            // Shader
            std::uint32_t m_shaderProgram;

            // Buffer
            std::uint32_t m_shadowFramebuffer;
            std::uint32_t m_depthMapBuffer;
        };
    }
}