#include "Chicane/Runtime/Renderer/OpenGL/Layer/Scene/Mesh.hpp"

#include <GL/glew.h>

namespace Chicane
{
    namespace OpenGL
    {
        LSceneMesh::LSceneMesh()
            : Super("Engine_Scene_Mesh"),
              m_textureManager(Box::getTextureManager()),
              m_modelManager(Box::getModelManager())
        {
            loadEvents();
        }

        LSceneMesh::~LSceneMesh()
        {
            destroyTextureData();
            destroyModelData();
            destroyShaders();
        }

        bool LSceneMesh::onInit()
        {
            buildShaders();
            buildTextureData();
            buildModelData();

            return true;
        }

        bool LSceneMesh::onSetup()
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

            glBindTextureUnit(0, m_textureBuffer);
            glBindVertexArray(m_modelVertexArray);

            return true;
        }

        void LSceneMesh::onRender(void* outData)
        {
            if (!is(RendererLayerStatus::Running))
            {
                return;
            }

            glUseProgram(m_shaderProgram);

            for (const String& id : m_textureManager->getActiveIds())
            {
                glTextureSubImage3D(
                    m_textureBuffer,
                    0,
                    0,
                    0,
                    m_textureManager->getIndex(id),
                    512,
                    512,
                    1,
                    GL_RGBA,
                    GL_UNSIGNED_BYTE,
                    m_textureManager->getData(id).getPixels()
                );
            }

            for (const String& id : m_modelManager->getActiveIds())
            {
                const Box::ModelParsed& data = m_modelManager->getData(id);

                glDrawElementsInstancedBaseVertexBaseInstance(
                    GL_TRIANGLES,
                    data.indexCount,
                    GL_UNSIGNED_INT,
                    (void*)(sizeof(uint32_t) * data.firstIndex),
                    m_modelManager->getUseCount(id),
                    0, // vertexOffset
                    m_modelManager->getFirstUse(id)
                );
            }
        }

        bool LSceneMesh::onCleanup()
        {
            return true;
        }

        void LSceneMesh::loadEvents()
        {
            if (!is(RendererLayerStatus::Offline))
            {
                return;
            }

            m_textureManager->watchChanges([&](Box::ManagerEventType inEvent) {
                if (inEvent != Box::ManagerEventType::Activation)
                {
                    return;
                }

                if (is(RendererLayerStatus::Offline))
                {
                    init();

                    return;
                }

                buildTextureData();
            });

            m_modelManager->watchChanges([&](Box::ManagerEventType inEvent) {
                if (inEvent != Box::ManagerEventType::Use)
                {
                    return;
                }

                if (is(RendererLayerStatus::Offline))
                {
                    init();

                    return;
                }

                buildModelData();
            });
        }

        void LSceneMesh::buildShaders()
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

        void LSceneMesh::destroyShaders()
        {
            glDeleteProgram(m_shaderProgram);
        }

        void LSceneMesh::buildTextureData()
        {
            glCreateTextures(GL_TEXTURE_2D_ARRAY, 1, &m_textureBuffer);
            glTextureStorage3D(m_textureBuffer, 1, GL_RGBA8, 512, 512, Box::Texture::MAX_COUNT);
        }

        void LSceneMesh::destroyTextureData()
        {
            glDeleteTextures(1, &m_textureBuffer);
        }

        void LSceneMesh::buildModelVertexArray()
        {
            glGenVertexArrays(1, &m_modelVertexArray);
            glBindVertexArray(m_modelVertexArray);
        }

        void LSceneMesh::buildModelVertexBuffer()
        {
            const std::vector<Vertex>& vertices = m_modelManager->getVertices();

            glGenBuffers(1, &m_modelVertexBuffer);
            glBindBuffer(GL_ARRAY_BUFFER, m_modelVertexBuffer);
            glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * vertices.size(), vertices.data(), GL_DYNAMIC_DRAW);

            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, position));
            glEnableVertexAttribArray(0);

            glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, color));
            glEnableVertexAttribArray(1);

            glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, uv));
            glEnableVertexAttribArray(2);

            glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
            glEnableVertexAttribArray(3);
        }

        void LSceneMesh::buildModelIndexBuffer()
        {
            const std::vector<std::uint32_t>& indices = m_modelManager->getIndices();

            glGenBuffers(1, &m_modelIndexBuffer);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_modelIndexBuffer);
            glBufferData(
                GL_ELEMENT_ARRAY_BUFFER,
                sizeof(std::uint32_t) * indices.size(),
                indices.data(),
                GL_DYNAMIC_DRAW
            );
        }

        void LSceneMesh::buildModelData()
        {
            buildModelVertexArray();
            buildModelVertexBuffer();
            buildModelIndexBuffer();
        }

        void LSceneMesh::destroyModelData()
        {
            glDeleteVertexArrays(1, &m_modelVertexArray);
            glDeleteBuffers(1, &m_modelVertexBuffer);
            glDeleteBuffers(1, &m_modelIndexBuffer);
        }

        void LSceneMesh::rebuildModelData()
        {
            destroyModelData();
            buildModelData();
        }
    }
}