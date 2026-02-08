#pragma once

#include "Chicane/Renderer.hpp"
#include "Chicane/Renderer/Layer.hpp"

namespace Editor
{
    class OpenGLLGrid : public Chicane::Renderer::Layer<>
    {
    public:
        OpenGLLGrid();
        ~OpenGLLGrid();

    public:
        bool onInit() override;
        void onRender(const Chicane::Renderer::Frame& inFrame, void* inData) override;
        void onCleanup() override;

    private:
        void buildShader();
        void destroyShader();

        void buildVertexArray();
        void destroyVertexArray();

    private:
        // Shader
        std::uint32_t m_shaderProgram;

        // Buffer
        std::uint32_t m_vertexArray;
    };
}