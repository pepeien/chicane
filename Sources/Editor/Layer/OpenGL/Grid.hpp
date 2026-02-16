#pragma once

#include <Chicane/Renderer/Frame.hpp>
#include <Chicane/Renderer/Layer.hpp>

namespace Editor
{
    class OpenGLLGrid : public Chicane::Renderer::Layer
    {
    public:
        OpenGLLGrid();

    public:
        void onInit() override;
        void onDestruction() override;

        void onRender(const Chicane::Renderer::Frame& inFrame, void* inData) override;
        void onEndRender() override;

    private:
        void buildShader();
        void destroyShader();

        void buildVertexArray();
        void destroyVertexArray();

        void buildViewport();

    private:
        // Shader
        std::uint32_t m_shaderProgram;

        // Buffer
        std::uint32_t m_vertexArray;
    };
}