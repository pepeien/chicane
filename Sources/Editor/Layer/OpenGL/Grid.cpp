#include "Layer/OpenGL/Grid.hpp"

#include <GL/glew.h>

#include "Chicane/Core/FileSystem.hpp"

namespace Editor
{
    OpenGLLGrid::OpenGLLGrid()
        : Layer("Editor_Scene_Grid")
    {}

    OpenGLLGrid::~OpenGLLGrid()
    {
        destroyShader();
    }

    bool OpenGLLGrid::onInit()
    {
        buildShader();

        return true;
    }

    void OpenGLLGrid::onRender(const Chicane::Renderer::Frame& inFrame, void* inData)
    {
        glUseProgram(m_shaderProgram);

        glEnable(GL_DEPTH_TEST);
        glDepthMask(GL_TRUE);
        glDepthFunc(GL_LEQUAL);

        glEnable(GL_CULL_FACE);
        glFrontFace(GL_CCW);
        glCullFace(GL_BACK);

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        glDrawArrays(GL_TRIANGLES, 0, 6);
    }

    void OpenGLLGrid::onCleanup()
    {
        glDisable(GL_DEPTH_TEST);
        glDisable(GL_CULL_FACE);
        glDisable(GL_BLEND);
    }

    void OpenGLLGrid::buildShader()
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

    void OpenGLLGrid::destroyShader()
    {
        glDeleteProgram(m_shaderProgram);
    }
}