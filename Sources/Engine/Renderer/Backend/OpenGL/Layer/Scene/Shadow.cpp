#include "Chicane/Renderer/Backend/OpenGL/Layer/Scene/Shadow.hpp"

#include <GL/glew.h>

#include "Chicane/Core/FileSystem.hpp"

namespace Chicane
{
    namespace Renderer
    {
        OpenGLLSceneShadow::OpenGLLSceneShadow()
            : Layer("Engine_Scene_Shadow"),
              m_viewport({})
        {
            m_viewport.size = Vec2(1024, 1024);
        }

        OpenGLLSceneShadow::~OpenGLLSceneShadow()
        {
            deleteChildren();
            destroyShader();
            destroyShadowMap();
        }

        bool OpenGLLSceneShadow::onInit()
        {
            buildShader();
            buildShadowMap();

            return true;
        }

        void OpenGLLSceneShadow::onRender(const Frame& inFrame, void* inData)
        {
            glUseProgram(m_shaderProgram);

            glBindFramebuffer(GL_FRAMEBUFFER, m_shadowFramebuffer);

            glEnable(GL_DEPTH_TEST);
            glDepthMask(GL_TRUE);
            glDepthFunc(GL_LESS);

            glEnable(GL_CULL_FACE);
            glFrontFace(GL_CW);
            glCullFace(GL_FRONT);

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

            glBindFramebuffer(GL_FRAMEBUFFER, 0);

            glActiveTexture(GL_TEXTURE3);
            glBindTexture(GL_TEXTURE_2D, m_depthMapBuffer);
        }

        void OpenGLLSceneShadow::onCleanup()
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
            glTextureStorage2D(m_depthMapBuffer, 1, GL_DEPTH_COMPONENT24, m_viewport.size.x, m_viewport.size.y);
            glTextureParameteri(m_depthMapBuffer, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            glTextureParameteri(m_depthMapBuffer, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            glTextureParameteri(m_depthMapBuffer, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
            glTextureParameteri(m_depthMapBuffer, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

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