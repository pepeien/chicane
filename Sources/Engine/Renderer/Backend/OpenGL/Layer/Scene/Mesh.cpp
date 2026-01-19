#include "Chicane/Renderer/Backend/OpenGL/Layer/Scene/Mesh.hpp"

#include <GL/glew.h>

#include "Chicane/Core/Log.hpp"

namespace Chicane
{
    namespace Renderer
    {
        OpenGLLSceneMesh::OpenGLLSceneMesh()
            : Layer("Engine_Scene_Mesh")
        {
            init();
        }

        OpenGLLSceneMesh::~OpenGLLSceneMesh()
        {
            destroyShader();
            destroyModelData();
        }

        bool OpenGLLSceneMesh::onInit()
        {
            buildShader();

            buildModelVertexArray();
            buildModelVertexBuffer();
            buildModelIndexBuffer();

            return true;
        }

        bool OpenGLLSceneMesh::onSetup(const Frame& inFrame)
        {
            // Depth Test
            glDepthMask(GL_TRUE);
            glDepthFunc(GL_LESS);

            // Blend
            glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ZERO);
            glBlendEquation(GL_FUNC_ADD);

            // Face Culling
            glFrontFace(GL_CCW);
            glCullFace(GL_BACK);

            const Vertex::List& vertices = inFrame.getVertices3D();
            glBindBuffer(GL_ARRAY_BUFFER, m_modelVertexBuffer);
            glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(Vertex) * vertices.size(), vertices.data());

            const Vertex::Indices& indices = inFrame.getIndices3D();
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_modelIndexBuffer);
            glBufferSubData(
                GL_ELEMENT_ARRAY_BUFFER,
                0,
                sizeof(Vertex::Index) * indices.size(),
                indices.data()
            );

            glBindVertexArray(m_modelVertexArray);

            glUseProgram(m_shaderProgram);

            return true;
        }

        void OpenGLLSceneMesh::onRender(const Frame& inFrame)
        {
            std::uint32_t instance = 0U;
            for (const Draw3D& draw : inFrame.getDraws3D())
            {
                glDrawElementsInstancedBaseInstance(
                    GL_TRIANGLES,
                    draw.indexCount,
                    GL_UNSIGNED_INT,
                    (void*)(sizeof(uint32_t) * draw.indexStart),
                    1,
                    instance
                );

                instance++;
            }
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

        void OpenGLLSceneMesh::buildModelVertexArray()
        {
            glGenVertexArrays(1, &m_modelVertexArray);
            glBindVertexArray(m_modelVertexArray);
        }

        void OpenGLLSceneMesh::buildModelVertexBuffer()
        {
            glGenBuffers(1, &m_modelVertexBuffer);
            glBindBuffer(GL_ARRAY_BUFFER, m_modelVertexBuffer);
            glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * 2000000, NULL, GL_DYNAMIC_DRAW);

            glVertexAttribPointer(
                0,
                3,
                GL_FLOAT,
                GL_FALSE,
                sizeof(Vertex),
                (void*)offsetof(Vertex, position)
            );
            glEnableVertexAttribArray(0);

            glVertexAttribPointer(
                1,
                4,
                GL_FLOAT,
                GL_FALSE,
                sizeof(Vertex),
                (void*)offsetof(Vertex, color)
            );
            glEnableVertexAttribArray(1);

            glVertexAttribPointer(
                2,
                2,
                GL_FLOAT,
                GL_FALSE,
                sizeof(Vertex),
                (void*)offsetof(Vertex, uv)
            );
            glEnableVertexAttribArray(2);

            glVertexAttribPointer(
                3,
                3,
                GL_FLOAT,
                GL_FALSE,
                sizeof(Vertex),
                (void*)offsetof(Vertex, normal)
            );
            glEnableVertexAttribArray(3);
        }

        void OpenGLLSceneMesh::buildModelIndexBuffer()
        {
            glGenBuffers(1, &m_modelIndexBuffer);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_modelIndexBuffer);
            glBufferData(
                GL_ELEMENT_ARRAY_BUFFER,
                sizeof(std::uint32_t) * 2000000,
                NULL,
                GL_DYNAMIC_DRAW
            );
        }

        void OpenGLLSceneMesh::destroyModelData()
        {
            glDeleteVertexArrays(1, &m_modelVertexArray);
            glDeleteBuffers(1, &m_modelVertexBuffer);
            glDeleteBuffers(1, &m_modelIndexBuffer);
        }
    }
}