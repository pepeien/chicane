#include "Layer/OpenGL/Grid.hpp"

#include <Chicane/Renderer/Backend/OpenGL.hpp>

namespace Editor
{
    OpenGLLUI::OpenGLLUI()
        : Layer("Editor_Scene_Grid")
    {}

    void OpenGLLUI::onInit()
    {
        buildShader();
        buildVertexArray();
        buildViewport();
    }

    void OpenGLLUI::onDestruction()
    {
        destroyVertexArray();
        destroyShader();
    }

    void OpenGLLUI::onRender(const Chicane::Renderer::Frame& inFrame, void* inData)
    {
        Chicane::Renderer::OpenGLBackend* backend = getBackend<Chicane::Renderer::OpenGLBackend>();

        backend->useViewport(this);

        backend->useProgram(m_shaderProgram);

        Chicane::Renderer::Depth depth;
        depth.bCanWrite = true;
        depth.compare   = Chicane::Renderer::DepthCompare::LessOrEqual;
        backend->enableDepth(depth);

        Chicane::Renderer::Culling culling;
        culling.frontFace = Chicane::Renderer::CullingFrontFace::CounterClockwise;
        culling.mode      = Chicane::Renderer::CullingMode::Back;
        backend->enableCulling(culling);

        Chicane::Renderer::Blending blending;
        blending.source      = Chicane::Renderer::BlendingFactor::SrcAlpha;
        blending.destination = Chicane::Renderer::BlendingFactor::OneMinusSrcAlpha;
        backend->enableBlending(blending);

        Chicane::Renderer::DrawPoly draw;
        draw.topology   = Chicane::Renderer::DrawPolyTopology::TriangleList;
        draw.indexCount = 6U;
        draw.indexStart = 0U;
        backend->drawPolyArrays(draw, m_vertexArray);
    }

    void OpenGLLUI::onEndRender()
    {
        Chicane::Renderer::OpenGLBackend* backend = getBackend<Chicane::Renderer::OpenGLBackend>();
        backend->disableDepth();
        backend->disableCulling();
        backend->disableBlending();
    }

    void OpenGLLUI::buildShader()
    {
        Chicane::Renderer::Shader::List shaders;

        Chicane::Renderer::Shader vertex;
        vertex.type   = Chicane::Renderer::ShaderType::Vertex;
        vertex.source = "Contents/Editor/Shaders/OpenGL/Grid.overt";
        shaders.push_back(vertex);

        Chicane::Renderer::Shader fragment;
        fragment.type   = Chicane::Renderer::ShaderType::Fragment;
        fragment.source = "Contents/Editor/Shaders/OpenGL/Grid.ofrag";
        shaders.push_back(fragment);

        m_shaderProgram = getBackend<Chicane::Renderer::OpenGLBackend>()->initShader(shaders);
    }

    void OpenGLLUI::destroyShader()
    {
        getBackend<Chicane::Renderer::OpenGLBackend>()->destroyProgram(m_shaderProgram);
    }

    void OpenGLLUI::buildVertexArray()
    {
        m_vertexArray = getBackend<Chicane::Renderer::OpenGLBackend>()->initVertexArray(1);
    }

    void OpenGLLUI::destroyVertexArray()
    {
        getBackend<Chicane::Renderer::OpenGLBackend>()->destroyVertexArray(m_vertexArray);
    }

    void OpenGLLUI::buildViewport()
    {
        Chicane::Renderer::ViewportSettings viewport;
        viewport.width  = "85vw";
        viewport.height = "80vh";

        setViewport(viewport);

        for (Layer* layer :
             m_backend->findLayers([](const Layer* inLayer)
                                   { return inLayer->getId().contains(Chicane::Renderer::SCENE_LAYER_ID); }))
        {
            layer->setViewport(viewport);
        }
    }
}