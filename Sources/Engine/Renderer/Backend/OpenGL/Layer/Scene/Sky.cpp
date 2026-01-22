#include "Chicane/Renderer/Backend/OpenGL/Layer/Scene/Sky.hpp"

#include <GL/glew.h>

#include "Chicane/Core/FileSystem.hpp"

namespace Chicane
{
    namespace Renderer
    {
        OpenGLLSceneSky::OpenGLLSceneSky()
            : Layer("Engine_Scene_Sky")
        {}

        OpenGLLSceneSky::~OpenGLLSceneSky()
        {
            destroyShader();
            destroyTextureData();
        }

        bool OpenGLLSceneSky::onInit()
        {
            buildShader();
            buildTextureData();

            return true;
        }

        void OpenGLLSceneSky::onLoad(const DrawSky& inResource)
        {
            if (inResource.textures.empty() || inResource.model.id == Draw::UnknownId)
            {
                return;
            }

            DrawTexture::List textures = inResource.textures;
            textures.at(0).image.rotate(90.0f);  // Right
            textures.at(1).image.rotate(-90.0f); // Left
            textures.at(2).image.rotate(180.0f); // Front

            int side = 0;
            for (const DrawTexture& texture : textures)
            {
                glTextureSubImage3D(
                    m_texturesBuffer,
                    0,
                    0,
                    0,
                    side,
                    texture.image.getWidth(),
                    texture.image.getHeight(),
                    1,
                    GL_RGBA,
                    GL_UNSIGNED_BYTE,
                    texture.image.getPixels()
                );

                side++;
            }
        }

        bool OpenGLLSceneSky::onSetup(const Frame& inFrame)
        {
            if (inFrame.getSkyInstance().model.id == Draw::UnknownId)
            {
                return false;
            }

            return true;
        }

        void OpenGLLSceneSky::onRender(const Frame& inFrame)
        {
            glUseProgram(m_shaderProgram);

            glEnable(GL_DEPTH_TEST);
            glDepthMask(GL_TRUE);
            glDepthFunc(GL_LESS);

            glEnable(GL_CULL_FACE);
            glCullFace(GL_FRONT);
            glFrontFace(GL_CCW);

            glClear(GL_DEPTH_BUFFER_BIT);

            glBindTexture(GL_TEXTURE_CUBE_MAP, m_texturesBuffer);

            const DrawPoly& draw = inFrame.getSkyInstance().model;
            glDrawElementsBaseVertex(
                GL_TRIANGLES,
                draw.indexCount,
                GL_UNSIGNED_INT,
                (void*)(sizeof(uint32_t) * draw.indexStart),
                draw.vertexStart
            );
        }

        void OpenGLLSceneSky::onCleanup()
        {
            glDisable(GL_DEPTH_TEST);
            glDisable(GL_CULL_FACE);
        }

        void OpenGLLSceneSky::buildShader()
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

        void OpenGLLSceneSky::destroyShader()
        {
            glDeleteProgram(m_shaderProgram);
        }

        void OpenGLLSceneSky::buildTextureData()
        {
            glCreateTextures(GL_TEXTURE_CUBE_MAP, 1, &m_texturesBuffer);
            glTextureStorage2D(m_texturesBuffer, 1, GL_RGBA8, 512, 512);

            // Filters
            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        }

        void OpenGLLSceneSky::destroyTextureData()
        {
            glDeleteTextures(1, &m_texturesBuffer);
        }
    }
}