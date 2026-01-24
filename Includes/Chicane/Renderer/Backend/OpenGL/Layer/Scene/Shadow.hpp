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
            OpenGLLSceneShadow();
            ~OpenGLLSceneShadow();

        public:
            bool onInit() override;
            void onRender(const Frame& inFrame, void* inData) override;
            void onCleanup() override;

        private:
            void buildShader();
            void destroyShader();

            void buildShadowMap();
            void destroyShadowMap();

        private:
            // Viewport
            Viewport      m_viewport;

            // Shader
            std::uint32_t m_shaderProgram;

            // Buffer
            std::uint32_t m_shadowFramebuffer;
            std::uint32_t m_depthMapBuffer;
        };
    }
}