#include "Chicane/Renderer/Backend/OpenGL/Layer/Scene/Mesh.hpp"

#include <GL/glew.h>

#include "Chicane/Core/FileSystem.hpp"

namespace Chicane
{
    namespace Renderer
    {
        OpenGLLSceneMesh::OpenGLLSceneMesh()
            : Layer("Engine_Scene_Mesh")
        {}

        OpenGLLSceneMesh::~OpenGLLSceneMesh()
        {
            deleteChildren();
            destroyShader();
        }

        bool OpenGLLSceneMesh::onInit()
        {
            buildShader();

            return true;
        }

        void OpenGLLSceneMesh::onRender(const Frame& inFrame, void* inData)
        {
            glUseProgram(m_shaderProgram);

            glEnable(GL_DEPTH_TEST);
            glDepthMask(GL_TRUE);
            glDepthFunc(GL_LESS);

            glEnable(GL_CULL_FACE);
            glFrontFace(GL_CCW);
            glCullFace(GL_BACK);

            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

            glClear(GL_DEPTH_BUFFER_BIT);

            for (const DrawPoly& draw : inFrame.get3DDraws())
            {
                if (draw.instanceCount == 0)
                {
                    continue;
                }

                glDrawElementsInstancedBaseVertexBaseInstance(
                    GL_TRIANGLES,
                    draw.indexCount,
                    GL_UNSIGNED_INT,
                    (void*)(sizeof(uint32_t) * draw.indexStart),
                    draw.instanceCount,
                    draw.vertexStart,
                    draw.instanceStart
                );
            }
        }

        void OpenGLLSceneMesh::onCleanup()
        {
            glDisable(GL_DEPTH_TEST);
            glDisable(GL_CULL_FACE);
            glDisable(GL_BLEND);
        }

        void OpenGLLSceneMesh::buildShader()
        {
            GLint result = GL_FALSE;

            // Vertex
            const std::vector<char> vertexShaderCode =
                FileSystem::read("Contents/Engine/Shaders/OpenGL/Scene/Mesh.overt");

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
                FileSystem::read("Contents/Engine/Shaders/OpenGL/Scene/Mesh.ofrag");

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

        void OpenGLLSceneMesh::destroyShader()
        {
            glDeleteProgram(m_shaderProgram);
        }
    }
}