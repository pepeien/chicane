#include "Chicane/Renderer/Backend/OpenGL/Layer/Grid.hpp"

#include <GL/glew.h>

#include "Chicane/Core/FileSystem.hpp"

namespace Chicane
{
    namespace Renderer
    {
        OpenGLLGrid::OpenGLLGrid()
            : Layer("Engine_Grid")
        {
            init();
        }

        OpenGLLGrid::~OpenGLLGrid()
        {
            destroyShader();
            destroyPrimitiveData();
            destroySizeData();
        }

        bool OpenGLLGrid::onInit()
        {
            buildShader();

            buildPrimitiveVertexArray();
            buildPrimitiveVertexBuffer();
            buildPrimitiveIndexBuffer();
            buildSizeData();

            return true;
        }

        bool OpenGLLGrid::onSetup(const Frame& inFrame)
        {
            const Vertex::List& vertices = inFrame.getVertices2D();
            glBindBuffer(GL_ARRAY_BUFFER, m_primitiveVertexBuffer);
            glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(Vertex) * vertices.size(), vertices.data());

            const Vertex::Indices& indices = inFrame.getIndices2D();
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_primitiveIndexBuffer);
            glBufferSubData(
                GL_ELEMENT_ARRAY_BUFFER,
                0,
                sizeof(Vertex::Index) * indices.size(),
                indices.data()
            );

            glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_sizeBuffer);
            glBufferSubData(
                GL_SHADER_STORAGE_BUFFER,
                0,
                sizeof(Draw2DInstance) * inFrame.getInstances2D().size(),
                inFrame.getInstances2D().data()
            );

            glBindVertexArray(m_primitiveVertexArray);

            glUseProgram(m_shaderProgram);

            return true;
        }

        void OpenGLLGrid::onRender(const Frame& inFrame)
        {
            std::uint32_t instanceStart = 0U;
            for (const Draw<Draw2DInstance>& draw : inFrame.getDraws2D())
            {
                glDrawElementsInstancedBaseVertexBaseInstance(
                    GL_TRIANGLES,
                    draw.indexCount,
                    GL_UNSIGNED_INT,
                    (void*)(sizeof(uint32_t) * draw.indexStart),
                    draw.instances.size(),
                    draw.vertexStart,
                    instanceStart
                );

                instanceStart += draw.instances.size();
            }
        }

        void OpenGLLGrid::buildShader()
        {
            GLint result = GL_FALSE;

            // Vertex
            const std::vector<char> vertexShaderCode =
                FileSystem::read("Contents/Engine/Shaders/OpenGL/Grid.overt");

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
                FileSystem::read("Contents/Engine/Shaders/OpenGL/Grid.ofrag");

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

        void OpenGLLGrid::buildPrimitiveVertexArray()
        {
            glGenVertexArrays(1, &m_primitiveVertexArray);
            glBindVertexArray(m_primitiveVertexArray);
        }

        void OpenGLLGrid::buildPrimitiveVertexBuffer()
        {
            glGenBuffers(1, &m_primitiveVertexBuffer);
            glBindBuffer(GL_ARRAY_BUFFER, m_primitiveVertexBuffer);
            glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * 1000, NULL, GL_DYNAMIC_DRAW);

            glVertexAttribPointer(
                0,
                3,
                GL_FLOAT,
                GL_FALSE,
                sizeof(Vertex),
                (void*)offsetof(Vertex, position)
            );
            glEnableVertexAttribArray(0);

            glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, color));
            glEnableVertexAttribArray(1);

            glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, uv));
            glEnableVertexAttribArray(2);

            glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
            glEnableVertexAttribArray(3);
        }

        void OpenGLLGrid::buildPrimitiveIndexBuffer()
        {
            glGenBuffers(1, &m_primitiveIndexBuffer);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_primitiveIndexBuffer);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(std::uint32_t) * 1000, NULL, GL_DYNAMIC_DRAW);
        }

        void OpenGLLGrid::destroyPrimitiveData()
        {
            glDeleteVertexArrays(1, &m_primitiveVertexArray);
            glDeleteBuffers(1, &m_primitiveVertexBuffer);
            glDeleteBuffers(1, &m_primitiveIndexBuffer);
        }

        void OpenGLLGrid::buildSizeData()
        {
            glGenBuffers(1, &m_sizeBuffer);
            glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_sizeBuffer);
            glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(Draw3DInstance) * 1000, NULL, GL_DYNAMIC_DRAW);
            glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, m_sizeBuffer);
        }

        void OpenGLLGrid::destroySizeData()
        {
            glDeleteBuffers(1, &m_sizeBuffer);
        }
    }
}