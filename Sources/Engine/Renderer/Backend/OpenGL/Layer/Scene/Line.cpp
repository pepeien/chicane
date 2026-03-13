#include "Chicane/Renderer/Backend/OpenGL/Layer/Scene/Line.hpp"

#include <glad/gl.h>

#include "Chicane/Core/FileSystem.hpp"

#include "Chicane/Renderer/Backend/OpenGL.hpp"

namespace Chicane
{
    namespace Renderer
    {
        OpenGLLSceneLine::OpenGLLSceneLine()
            : Layer(SCENE_LINE_LAYER_ID)
        {}

        void OpenGLLSceneLine::onInit()
        {
            buildShader();
        }

        void OpenGLLSceneLine::onDestruction()
        {
            destroyShader();
        }

        bool OpenGLLSceneLine::onBeginRender(const Frame& inFrame)
        {
            if (!inFrame.hasDraws(DrawPolyType::e3D, DrawPolyMode::Line))
            {
                return false;
            }

            return true;
        }

        void OpenGLLSceneLine::onRender(const Frame& inFrame, void* inData)
        {
            glUseProgram(m_shaderProgram);

            glEnable(GL_DEPTH_TEST);
            glDepthMask(GL_TRUE);
            glDepthFunc(GL_LEQUAL);

            glDisable(GL_CULL_FACE);
            glFrontFace(GL_CCW);

            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

            Viewport viewport = getBackend<OpenGLBackend>()->getGLViewport(this);
            glViewport(viewport.position.x, viewport.position.y, viewport.size.x, viewport.size.y);

            for (const DrawPoly& draw : inFrame.getDraws(DrawPolyType::e3D, DrawPolyMode::Line))
            {
                glDrawElementsInstancedBaseVertexBaseInstance(
                    GL_TRIANGLES,
                    draw.indexCount,
                    GL_UNSIGNED_INT,
                    (void*)(sizeof(Vertex::Index) * draw.indexStart),
                    draw.instanceCount,
                    draw.vertexStart,
                    draw.instanceStart
                );
            }
        }

        void OpenGLLSceneLine::onEndRender()
        {
            glDisable(GL_DEPTH_TEST);
            glDisable(GL_BLEND);
        }

        void OpenGLLSceneLine::buildShader()
        {
            GLint result = GL_FALSE;

            // Vertex
            const std::vector<char> vertexShaderCode =
                FileSystem::read("Assets/Engine/Shaders/OpenGL/Scene/Line.overt");

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
                FileSystem::read("Assets/Engine/Shaders/OpenGL/Scene/Line.ofrag");

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

        void OpenGLLSceneLine::destroyShader()
        {
            glDeleteProgram(m_shaderProgram);
        }
    }
}