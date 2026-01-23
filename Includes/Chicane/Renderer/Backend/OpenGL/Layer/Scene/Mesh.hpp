#pragma once

#include "Chicane/Renderer.hpp"
#include "Chicane/Renderer/Layer.hpp"

namespace Chicane
{
    namespace Renderer
    {
        class CHICANE_RENDERER OpenGLLSceneMesh : public Layer
        {
        public:
            OpenGLLSceneMesh();
            ~OpenGLLSceneMesh();

        public:
            bool onInit() override;
            void onRender(const Frame& inFrame, void* inData) override;
            void onCleanup() override;

        private:
            void buildShader();
            void destroyShader();

        private:
            // Shader
            std::uint32_t m_shaderProgram;
        };
    }
}