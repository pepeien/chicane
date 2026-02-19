#include "Layer/OpenGL/Grid.hpp"

#include <GL/glew.h>

#include <Chicane/Core/FileSystem.hpp>

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
        Chicane::Renderer::Viewport viewport = getBackend<Chicane::Renderer::OpenGLBackend>()->getGLViewport(this);
        glViewport(viewport.position.x, viewport.position.y, viewport.size.x, viewport.size.y);

        glUseProgram(m_shaderProgram);

        glEnable(GL_DEPTH_TEST);
        glDepthMask(GL_TRUE);
        glDepthFunc(GL_LEQUAL);

        glEnable(GL_CULL_FACE);
        glFrontFace(GL_CCW);
        glCullFace(GL_BACK);

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        glBindVertexArray(m_vertexArray);

        glDrawArrays(GL_TRIANGLES, 0, 6);
    }

    void OpenGLLUI::onEndRender()
    {
        glDisable(GL_DEPTH_TEST);
        glDisable(GL_CULL_FACE);
        glDisable(GL_BLEND);
    }

    void OpenGLLUI::buildShader()
    {
        GLint result = GL_FALSE;

        // Vertex
        const std::vector<char> vertexShaderCode =
            Chicane::FileSystem::read("Contents/Editor/Shaders/OpenGL/Grid.overt");

        GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
        glShaderBinary(
            1,
            &vertexShader,
            GL_SHADER_BINARY_FORMAT_SPIR_V,
            vertexShaderCode.data(),
            vertexShaderCode.size()
        );
        glSpecializeShader(vertexShader, "main", 0, nullptr, nullptr);
        glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &result);
        if (!result)
        {
            throw std::runtime_error("Failed to load vertex shader");
        }

        result = GL_FALSE;

        // Fragment
        const std::vector<char> fragmentShaderCode =
            Chicane::FileSystem::read("Contents/Editor/Shaders/OpenGL/Grid.ofrag");

        GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderBinary(
            1,
            &fragmentShader,
            GL_SHADER_BINARY_FORMAT_SPIR_V,
            fragmentShaderCode.data(),
            fragmentShaderCode.size()
        );
        glSpecializeShader(fragmentShader, "main", 0, nullptr, nullptr);
        glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &result);
        if (!result)
        {
            throw std::runtime_error("Failed to load fragment shader");
        }

        result = GL_FALSE;

        // Shader Program
        m_shaderProgram = glCreateProgram();
        glAttachShader(m_shaderProgram, vertexShader);
        glAttachShader(m_shaderProgram, fragmentShader);
        glLinkProgram(m_shaderProgram);

        glGetProgramiv(m_shaderProgram, GL_LINK_STATUS, &result);
        if (!result)
        {
            throw std::runtime_error("Failed link shader program");
        }

        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);
    }

    void OpenGLLUI::destroyShader()
    {
        glDeleteProgram(m_shaderProgram);
    }

    void OpenGLLUI::buildVertexArray()
    {
        glCreateVertexArrays(1, &m_vertexArray);
    }

    void OpenGLLUI::destroyVertexArray()
    {
        glDeleteVertexArrays(1, &m_vertexArray);
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