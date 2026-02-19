#include "Chicane/Renderer/Backend/OpenGL/Layer/UI.hpp"

#include <GL/glew.h>

#include "Chicane/Core/FileSystem.hpp"

#include "Chicane/Renderer/Backend/OpenGL.hpp"

namespace Chicane
{
    namespace Renderer
    {
        OpenGLLUI::OpenGLLUI()
            : Layer(UI_LAYER_ID)
        {}

        void OpenGLLUI::onInit()
        {
            buildShader();
            buildPrimitiveVertexArray();
            buildPrimitiveVertexBuffer();
            buildPrimitiveIndexBuffer();
            buildInstanceData();
        }

        void OpenGLLUI::onDestruction()
        {
            destroyShader();
            destroyPrimitiveData();
            destroyInstanceData();
        }

        void OpenGLLUI::onLoad(DrawPolyType inType, const DrawPolyResource& inResource)
        {
            if (inType == DrawPolyType::e2D)
            {
                glNamedBufferSubData(
                    m_primitiveVertexBuffer,
                    0,
                    sizeof(Vertex) * inResource.getVertices().size(),
                    inResource.getVertices().data()
                );
                glNamedBufferSubData(
                    m_primitiveIndexBuffer,
                    0,
                    sizeof(Vertex::Index) * inResource.getIndices().size(),
                    inResource.getIndices().data()
                );
            }
        }

        bool OpenGLLUI::onBeginRender(const Frame& inFrame)
        {
            if (inFrame.getInstances2D().empty() || inFrame.get2DDraws().empty())
            {
                return false;
            }

            return true;
        }

        void OpenGLLUI::onRender(const Frame& inFrame, void* inData)
        {
            glUseProgram(m_shaderProgram);

            glClear(GL_DEPTH_BUFFER_BIT);

            glEnable(GL_DEPTH_TEST);
            glDepthMask(GL_TRUE);
            glDepthFunc(GL_LEQUAL);

            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

            glFrontFace(GL_CCW);

            glBindVertexArray(m_primitiveVertexArray);
            glVertexArrayElementBuffer(m_primitiveVertexArray, m_primitiveIndexBuffer);
            glVertexArrayVertexBuffer(m_primitiveVertexArray, 0, m_primitiveVertexBuffer, 0, sizeof(Vertex));

            glNamedBufferSubData(
                m_instanceBuffer,
                0,
                sizeof(DrawPoly2DInstance) * inFrame.getInstances2D().size(),
                inFrame.getInstances2D().data()
            );

            Viewport viewport = getBackend<OpenGLBackend>()->getGLViewport(this);
            glViewport(viewport.position.x, viewport.position.y, viewport.size.x, viewport.size.y);

            for (const DrawPoly& draw : inFrame.get2DDraws())
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

        void OpenGLLUI::onEndRender()
        {
            glDisable(GL_DEPTH_TEST);
            glDisable(GL_BLEND);
        }

        void OpenGLLUI::buildShader()
        {
            GLint result = GL_FALSE;

            // Vertex
            const std::vector<char> vertexShaderCode = FileSystem::read("Contents/Engine/Shaders/OpenGL/UI.overt");

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
            const std::vector<char> fragmentShaderCode = FileSystem::read("Contents/Engine/Shaders/OpenGL/UI.ofrag");

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

        void OpenGLLUI::buildPrimitiveVertexArray()
        {
            glCreateVertexArrays(1, &m_primitiveVertexArray);
        }

        void OpenGLLUI::buildPrimitiveVertexBuffer()
        {
            glCreateBuffers(1, &m_primitiveVertexBuffer);
            glNamedBufferData(
                m_primitiveVertexBuffer,
                m_backend->getResourceBudget(Resource::UIVertices),
                nullptr,
                GL_DYNAMIC_DRAW
            );

            // Position
            glEnableVertexArrayAttrib(m_primitiveVertexArray, 0);
            glVertexArrayAttribFormat(m_primitiveVertexArray, 0, 3, GL_FLOAT, GL_FALSE, offsetof(Vertex, position));
            glVertexArrayAttribBinding(m_primitiveVertexArray, 0, 0);

            // Color
            glEnableVertexArrayAttrib(m_primitiveVertexArray, 1);
            glVertexArrayAttribFormat(m_primitiveVertexArray, 1, 4, GL_FLOAT, GL_FALSE, offsetof(Vertex, color));
            glVertexArrayAttribBinding(m_primitiveVertexArray, 1, 0);

            // UV
            glEnableVertexArrayAttrib(m_primitiveVertexArray, 2);
            glVertexArrayAttribFormat(m_primitiveVertexArray, 2, 2, GL_FLOAT, GL_FALSE, offsetof(Vertex, uv));
            glVertexArrayAttribBinding(m_primitiveVertexArray, 2, 0);

            // Normal
            glEnableVertexArrayAttrib(m_primitiveVertexArray, 3);
            glVertexArrayAttribFormat(m_primitiveVertexArray, 3, 3, GL_FLOAT, GL_FALSE, offsetof(Vertex, normal));
            glVertexArrayAttribBinding(m_primitiveVertexArray, 3, 0);
        }

        void OpenGLLUI::buildPrimitiveIndexBuffer()
        {
            glCreateBuffers(1, &m_primitiveIndexBuffer);
            glNamedBufferData(
                m_primitiveIndexBuffer,
                m_backend->getResourceBudget(Resource::UIIndices),
                nullptr,
                GL_DYNAMIC_DRAW
            );
        }

        void OpenGLLUI::destroyPrimitiveData()
        {
            glDeleteVertexArrays(1, &m_primitiveVertexArray);
            glDeleteBuffers(1, &m_primitiveVertexBuffer);
            glDeleteBuffers(1, &m_primitiveIndexBuffer);
        }

        void OpenGLLUI::buildInstanceData()
        {
            glCreateBuffers(1, &m_instanceBuffer);
            glNamedBufferData(
                m_instanceBuffer,
                m_backend->getResourceBudget(Resource::UIInstances),
                nullptr,
                GL_DYNAMIC_DRAW
            );

            glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3, m_instanceBuffer);
        }

        void OpenGLLUI::destroyInstanceData()
        {
            glDeleteBuffers(1, &m_instanceBuffer);
        }
    }
}