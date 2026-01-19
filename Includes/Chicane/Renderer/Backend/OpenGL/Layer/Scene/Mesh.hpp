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
            bool onSetup(const Frame& inFrame) override;
            void onRender(const Frame& inFrame) override;

        private:
            void buildShader();
            void destroyShader();

            void buildModelVertexArray();
            void buildModelVertexBuffer();
            void buildModelIndexBuffer();
            void destroyModelData();

        private:
            // Shader
            std::uint32_t m_shaderProgram;

            // Model
            std::uint32_t m_modelVertexArray;
            std::uint32_t m_modelVertexBuffer;
            std::uint32_t m_modelIndexBuffer;
        };
    }
}