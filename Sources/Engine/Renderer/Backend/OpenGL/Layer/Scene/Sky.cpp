#include "Chicane/Renderer/Backend/OpenGL/Layer/Scene/Sky.hpp"

#include <GL/glew.h>

#include "Chicane/Core/FileSystem.hpp"

#include "Chicane/Renderer/Backend/OpenGL.hpp"

namespace Chicane
{
    namespace Renderer
    {
        OpenGLLSceneSky::OpenGLLSceneSky()
            : Layer(SCENE_SKY_LAYER_ID)
        {}

        void OpenGLLSceneSky::onInit()
        {
            buildShader();
            buildTextureData();
        }

        void OpenGLLSceneSky::onDestruction()
        {
            destroyShader();
            destroyTextureData();
        }

        void OpenGLLSceneSky::onLoad(const DrawSky& inResource)
        {
            glClearTexImage(m_texturesBuffer, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);

            if (inResource.textures.empty() || inResource.model.id == Draw::UnknownId)
            {
                return;
            }

            int side = 0;
            for (const DrawTexture& texture : inResource.textures)
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

        bool OpenGLLSceneSky::onBeginRender(const Frame& inFrame)
        {
            if (inFrame.getSkyInstance().model.id == Draw::UnknownId)
            {
                return false;
            }

            return true;
        }

        void OpenGLLSceneSky::onRender(const Frame& inFrame, void* inData)
        {
            glUseProgram(m_shaderProgram);

            glEnable(GL_CULL_FACE);
            glCullFace(GL_FRONT);
            glFrontFace(GL_CCW);

            glBindTextureUnit(1, m_texturesBuffer);

            Viewport viewport = getBackend<OpenGLBackend>()->getGLViewport(this);
            glViewport(viewport.position.x, viewport.position.y, viewport.size.x, viewport.size.y);

            const DrawPoly& draw = inFrame.getSkyInstance().model;
            glDrawElementsBaseVertex(
                GL_TRIANGLES,
                draw.indexCount,
                GL_UNSIGNED_INT,
                (void*)(sizeof(uint32_t) * draw.indexStart),
                draw.vertexStart
            );
        }

        void OpenGLLSceneSky::onEndRender()
        {
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

            glTextureParameteri(m_texturesBuffer, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTextureParameteri(m_texturesBuffer, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

            glTextureParameteri(m_texturesBuffer, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTextureParameteri(m_texturesBuffer, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            glTextureParameteri(m_texturesBuffer, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
        }

        void OpenGLLSceneSky::destroyTextureData()
        {
            glDeleteTextures(1, &m_texturesBuffer);
        }
    }
}