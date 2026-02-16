#include "Chicane/Renderer/Backend/OpenGL/Layer/Scene/Shadow.hpp"

#include <GL/glew.h>

#include "Chicane/Core/FileSystem.hpp"

namespace Chicane
{
    namespace Renderer
    {
        OpenGLLSceneShadow::OpenGLLSceneShadow()
            : Layer(ID)
        {
            m_viewport.size = Vec2(1024, 1024);
        }

        void OpenGLLSceneShadow::onInit()
        {
            buildShader();
            buildShadowMap();
        }

        void OpenGLLSceneShadow::onDestruction()
        {
            destroyShader();
            destroyShadowMap();
        }

        bool OpenGLLSceneShadow::onBeginRender(const Frame& inFrame)
        {
            if (inFrame.getInstances3D().empty() || inFrame.get3DDraws().empty())
            {
                return false;
            }

            return true;
        }

        void OpenGLLSceneShadow::onRender(const Frame& inFrame, void* inData)
        {
            Viewport viewport = getViewport();
            glViewport(viewport.position.x, viewport.position.y, viewport.size.x, viewport.size.y);

            glUseProgram(m_shaderProgram);

            glBindFramebuffer(GL_FRAMEBUFFER, m_shadowFramebuffer);

            glEnable(GL_DEPTH_TEST);
            glDepthMask(GL_TRUE);
            glDepthFunc(GL_LEQUAL);

            glEnable(GL_CULL_FACE);
            glFrontFace(GL_CW);
            glCullFace(GL_FRONT);

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

            glBindFramebuffer(GL_FRAMEBUFFER, 0);

            glBindTextureUnit(2, m_depthMapBuffer);
        }

        void OpenGLLSceneShadow::onEndRender()
        {
            glDisable(GL_DEPTH_TEST);
            glDisable(GL_CULL_FACE);
        }

        void OpenGLLSceneShadow::buildShader()
        {
            GLint result = GL_FALSE;

            // Vertex
            const std::vector<char> vertexShaderCode =
                FileSystem::read("Contents/Engine/Shaders/OpenGL/Scene/Shadow.overt");

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

            // Shader Program
            m_shaderProgram = glCreateProgram();
            glAttachShader(m_shaderProgram, vertexShader);
            glLinkProgram(m_shaderProgram);

            glGetProgramiv(m_shaderProgram, GL_LINK_STATUS, &result);
            if (!result)
            {
                throw std::runtime_error("Failed link shader program");
            }

            glDeleteShader(vertexShader);
        }

        void OpenGLLSceneShadow::destroyShader()
        {
            glDeleteProgram(m_shaderProgram);
        }

        void OpenGLLSceneShadow::buildShadowMap()
        {
            // Framebuffer
            glCreateFramebuffers(1, &m_shadowFramebuffer);

            // Depth Map
            glCreateTextures(GL_TEXTURE_2D, 1, &m_depthMapBuffer);
            glTextureStorage2D(m_depthMapBuffer, 1, GL_DEPTH_COMPONENT32F, m_viewport.size.x, m_viewport.size.y);

            glTextureParameteri(m_depthMapBuffer, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            glTextureParameteri(m_depthMapBuffer, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

            glTextureParameteri(m_depthMapBuffer, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
            glTextureParameteri(m_depthMapBuffer, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
            glTextureParameteri(m_depthMapBuffer, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_BORDER);

            // Attach
            glBindFramebuffer(GL_FRAMEBUFFER, m_shadowFramebuffer);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_depthMapBuffer, 0);
            glDrawBuffer(GL_NONE);
            glReadBuffer(GL_NONE);

            glBindFramebuffer(GL_FRAMEBUFFER, 0);
        }

        void OpenGLLSceneShadow::destroyShadowMap()
        {
            glDeleteTextures(1, &m_depthMapBuffer);
            glDeleteFramebuffers(1, &m_shadowFramebuffer);
        }
    }
}