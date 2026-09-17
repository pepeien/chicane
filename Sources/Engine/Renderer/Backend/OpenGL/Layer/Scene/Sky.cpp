#include "Chicane/Renderer/Backend/OpenGL/Layer/Scene/Sky.hpp"

#include <cmath>
#include <vector>

#include <glad/gl.h>

#include "Chicane/Core/FileSystem.hpp"
#include "Chicane/Core/Image.hpp"

#include "Chicane/Renderer/Backend/OpenGL.hpp"

namespace Chicane
{
    namespace Renderer
    {
        static float srgbToLinear(float inValue)
        {
            if (inValue <= 0.04045f)
            {
                return inValue / 12.92f;
            }

            return std::pow((inValue + 0.055f) / 1.055f, 2.4f);
        }
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

        void OpenGLLSceneSky::onLoad(const DrawSkyResource& inResource)
        {
            clearTextureData();

            if (inResource.isEmpty())
            {
                return;
            }

            updateTextureData(inResource.getDraw());

            glGenerateTextureMipmap(m_texturesBuffer);
        }

        bool OpenGLLSceneSky::onBeginRender(const Frame& inFrame)
        {
            if (inFrame.getSkyInstance().model.id <= Draw::InvalidId)
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

            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

            glBindTextureUnit(1, m_texturesBuffer);
            glProgramUniform1f(m_shaderProgram, 4, inFrame.getSkyInstance().exposure);

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
            const std::vector<char> vertexShaderCode = FileSystem::read("Assets/Engine/Shaders/OpenGL/Scene/Sky.overt");

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
                FileSystem::read("Assets/Engine/Shaders/OpenGL/Scene/Sky.ofrag");

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
            glTextureStorage2D(
                m_texturesBuffer,
                static_cast<GLsizei>(SKY_MIP_LEVELS),
                GL_RGBA16F,
                static_cast<GLsizei>(SKY_TEXTURE_SIZE),
                static_cast<GLsizei>(SKY_TEXTURE_SIZE)
            );

            glTextureParameteri(m_texturesBuffer, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTextureParameteri(m_texturesBuffer, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

            glTextureParameteri(m_texturesBuffer, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTextureParameteri(m_texturesBuffer, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            glTextureParameteri(m_texturesBuffer, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

            clearTextureData();

            for (OpenGLFrame& frame : getBackend<OpenGLBackend>()->frames)
            {
                frame.addObject(m_id, m_texturesBuffer);
            }
        }

        void OpenGLLSceneSky::clearTextureData()
        {
            for (std::uint32_t level = 0; level < SKY_MIP_LEVELS; level++)
            {
                glClearTexImage(m_texturesBuffer, static_cast<GLint>(level), GL_RGBA, GL_FLOAT, nullptr);
            }
        }

        void OpenGLLSceneSky::updateTextureData(const DrawSky& inValue)
        {
            std::vector<unsigned char> srgb(static_cast<std::size_t>(SKY_TEXTURE_SIZE) * SKY_TEXTURE_SIZE * 4u);
            std::vector<float>         linear(srgb.size());

            int side = 0;
            for (const DrawTexture& texture : inValue.textures)
            {
                if (const Image::Instance image = texture.getSampleImage())
                {
                    image->blit(srgb.data(), static_cast<int>(SKY_TEXTURE_SIZE), static_cast<int>(SKY_TEXTURE_SIZE));

                    for (std::size_t index = 0; index < linear.size(); index++)
                    {
                        const float channel = static_cast<float>(srgb[index]) / 255.0f;
                        const bool  bAlpha  = (index % 4u) == 3u;
                        linear[index]       = bAlpha ? channel : srgbToLinear(channel);
                    }

                    glTextureSubImage3D(
                        m_texturesBuffer,
                        0,
                        0,
                        0,
                        side,
                        static_cast<GLsizei>(SKY_TEXTURE_SIZE),
                        static_cast<GLsizei>(SKY_TEXTURE_SIZE),
                        1,
                        GL_RGBA,
                        GL_FLOAT,
                        linear.data()
                    );
                }

                side++;
            }
        }

        void OpenGLLSceneSky::destroyTextureData()
        {
            for (OpenGLFrame& frame : getBackend<OpenGLBackend>()->frames)
            {
                frame.removeObject(m_id);
            }
            glDeleteTextures(1, &m_texturesBuffer);
        }
    }
}