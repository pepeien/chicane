#include "Chicane/Runtime/Renderer/OpenGL/Layer/Scene/Sky.hpp"

#include "Chicane/Box/Asset/Sky/Parsed.hpp"
#include "Chicane/Core/Log.hpp"
#include "Chicane/Core/Math/Vertex.hpp"
#include "Chicane/Runtime/Application.hpp"
#include "Chicane/Runtime/Renderer/View.hpp"
#include "Chicane/Runtime/Scene.hpp"
#include "Chicane/Runtime/Scene/Actor/Sky.hpp"

namespace Chicane
{
    namespace OpenGL
    {
        LSceneSky::LSceneSky()
            : Super("Engine_Scene_Sky"),
              m_asset(nullptr),
              m_modelManager(Box::getModelManager()),
              m_textureManager(Box::getTextureManager())
        {
            loadEvents();
        }

        LSceneSky::~LSceneSky()
        {
            destroyTextureData();
            destroyModelData();
            destroyCameraData();
            destroyShaders();
        }

        bool LSceneSky::onInit()
        {
            if (!m_asset)
            {
                return false;
            }

            buildShaders();
            buildTextureData();
            buildModelData();
            buildCameraData();

            return true;
        }

        void LSceneSky::onRender(void* outData)
        {
            if (!is(RendererLayerStatus::Running))
            {
                return;
            }

            glBindTexture(GL_TEXTURE_CUBE_MAP, m_textureBuffer);

            glBindVertexArray(m_vao);

            CCamera*     camera     = static_cast<CCamera*>(outData);
            RendererView cameraData = camera->getData();

            glBindBuffer(GL_UNIFORM_BUFFER, m_cameraBuffer);
            glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(RendererView), &cameraData);

            glUseProgram(m_shaderProgram);

            glDrawElements(
                GL_TRIANGLES,
                static_cast<std::uint32_t>(m_modelManager->getInstance(m_asset->getModel()).indices.size()),
                GL_UNSIGNED_INT,
                0
            );
        }

        void LSceneSky::loadEvents()
        {
            if (!is(RendererLayerStatus::Offline))
            {
                return;
            }

            Application::watchScene([this](Scene* inLevel) {
                if (!inLevel)
                {
                    return;
                }

                inLevel->watchActors([this](const std::vector<Actor*>& inActors) {
                    const std::vector<ASky*> skies = Application::getScene()->getActors<ASky>();

                    if (skies.empty())
                    {
                        destroy();

                        return;
                    }

                    skies.front()->watchSky([this](const Chicane::Box::Sky* inSky) {
                        if (!is(RendererLayerStatus::Offline) || !inSky)
                        {
                            return;
                        }

                        m_asset = inSky;

                        init();
                    });
                });
            });
        }

        void LSceneSky::buildShaders()
        {
            GLint result = GL_FALSE;

            // Vertex
            const std::vector<char> vertexShaderCode =
                FileSystem::read("Contents/Engine/Shaders/OpenGL/Scene/Sky.overt");

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
                glDeleteShader(vertexShader);

                throw std::runtime_error("Failed to load vertex shader");
            }

            result = GL_FALSE;

            // Fragment
            const std::vector<char> fragmentShaderCode =
                FileSystem::read("Contents/Engine/Shaders/OpenGL/Scene/Sky.ofrag");

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
                glDeleteShader(fragmentShader);

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
                glDeleteShader(vertexShader);
                glDeleteShader(fragmentShader);

                throw std::runtime_error("Failed link shader program");
            }

            glDeleteShader(vertexShader);
            glDeleteShader(fragmentShader);
        }

        void LSceneSky::destroyShaders()
        {
            glDeleteProgram(m_shaderProgram);
        }

        void LSceneSky::buildTextureData()
        {
            glGenTextures(1, &m_textureBuffer);

            glBindTexture(GL_TEXTURE_CUBE_MAP, m_textureBuffer);

            // Filters
            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

            Box::SkyParsedSides sides = {};

            for (const auto& [side, texture] : m_asset->getSides())
            {
                m_textureManager->activate(texture);

                sides.insert(std::make_pair(side, m_textureManager->getInstance(texture)));
            }

            for (Box::SkySide side : Box::Sky::ORDER)
            {
                GLenum glSide = GL_TEXTURE_CUBE_MAP_POSITIVE_X_EXT;

                switch (side)
                {
                case Box::SkySide::Right:
                    glSide = GL_TEXTURE_CUBE_MAP_NEGATIVE_X;

                    break;

                case Box::SkySide::Left:
                    glSide = GL_TEXTURE_CUBE_MAP_POSITIVE_X;

                    break;

                case Box::SkySide::Up:
                    glSide = GL_TEXTURE_CUBE_MAP_POSITIVE_Z;

                    break;

                case Box::SkySide::Down:
                    glSide = GL_TEXTURE_CUBE_MAP_NEGATIVE_Z;

                    break;

                case Box::SkySide::Front:
                    glSide = GL_TEXTURE_CUBE_MAP_POSITIVE_Y;

                    break;

                case Box::SkySide::Back:
                    glSide = GL_TEXTURE_CUBE_MAP_NEGATIVE_Y;

                    break;
                }

                const Image& image = sides.at(side);

                glTexImage2D(
                    glSide,
                    0,
                    GL_RGB8,
                    image.getWidth(),
                    image.getHeight(),
                    0,
                    GL_RGBA,
                    GL_UNSIGNED_BYTE,
                    image.getPixels()
                );
            }
        }

        void LSceneSky::destroyTextureData()
        {
            glDeleteTextures(1, &m_textureBuffer);
        }

        void LSceneSky::buildModelVertexBuffer()
        {
            const std::vector<Vertex>& vertices = m_modelManager->getInstance(m_asset->getModel()).vertices;

            glGenBuffers(1, &m_modelVertexBuffer);
            glBindBuffer(GL_ARRAY_BUFFER, m_modelVertexBuffer);
            glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);

            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, position));
            glEnableVertexAttribArray(0);

            glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, color));
            glEnableVertexAttribArray(1);

            glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, uv));
            glEnableVertexAttribArray(2);

            glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
            glEnableVertexAttribArray(3);
        }

        void LSceneSky::buildModelIndexBuffer()
        {
            const std::vector<std::uint32_t>& indices = m_modelManager->getInstance(m_asset->getModel()).indices;

            glGenBuffers(1, &m_modelIndexBuffer);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_modelIndexBuffer);
            glBufferData(
                GL_ELEMENT_ARRAY_BUFFER,
                indices.size() * sizeof(std::uint32_t),
                indices.data(),
                GL_STATIC_DRAW
            );
        }

        void LSceneSky::buildModelData()
        {
            glGenVertexArrays(1, &m_vao);
            glBindVertexArray(m_vao);

            buildModelVertexBuffer();
            buildModelIndexBuffer();
        }

        void LSceneSky::destroyModelData()
        {
            glDeleteVertexArrays(1, &m_vao);
            glDeleteBuffers(1, &m_modelVertexBuffer);
            glDeleteBuffers(1, &m_modelIndexBuffer);
        }

        void LSceneSky::rebuildModelData()
        {
            destroyModelData();
            buildModelData();
        }

        void LSceneSky::buildCameraData()
        {
            glGenBuffers(1, &m_cameraBuffer);
            glBindBuffer(GL_UNIFORM_BUFFER, m_cameraBuffer);
            glBufferData(GL_UNIFORM_BUFFER, sizeof(RendererView), NULL, GL_DYNAMIC_DRAW);
            glBindBuffer(GL_UNIFORM_BUFFER, 0);

            glBindBufferBase(GL_UNIFORM_BUFFER, 0, m_cameraBuffer);
        }

        void LSceneSky::destroyCameraData()
        {
            glDeleteBuffers(1, &m_cameraBuffer);
        }
    }
}