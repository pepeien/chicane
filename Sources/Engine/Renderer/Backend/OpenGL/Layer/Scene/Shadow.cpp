#include "Chicane/Renderer/Backend/OpenGL/Layer/Scene/Shadow.hpp"

#include <glad/gl.h>

#include "Chicane/Core/FileSystem.hpp"

#include "Chicane/Renderer/Backend/OpenGL.hpp"

namespace Chicane
{
    namespace Renderer
    {
        OpenGLLSceneShadow::OpenGLLSceneShadow()
            : Layer(SCENE_SHADOW_LAYER_ID)
        {}

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
            if (!inFrame.hasDraws(DrawPolyType::e3D, DrawPolyMode::Fill))
            {
                return false;
            }

            return true;
        }

        void OpenGLLSceneShadow::onRender(const Frame& inFrame, void* inData)
        {
            glUseProgram(m_shaderProgram);

            glBindFramebuffer(GL_FRAMEBUFFER, m_shadowFramebuffer);

            glEnable(GL_DEPTH_TEST);
            glDepthMask(GL_TRUE);
            glDepthFunc(GL_LEQUAL);

            glEnable(GL_CULL_FACE);
            glFrontFace(GL_CW);
            glCullFace(GL_FRONT);

            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

            glEnable(GL_POLYGON_OFFSET_FILL);
            glPolygonOffset(2.0f, 4.0f);

            glClear(GL_DEPTH_BUFFER_BIT);

            Viewport viewport = getBackend<OpenGLBackend>()->getGLViewport(this);
            glViewport(viewport.position.x, viewport.position.y, SHADOW_MAP_WIDTH, SHADOW_MAP_HEIGHT);

            for (const DrawPoly& draw : inFrame.getDraws(DrawPolyType::e3D, DrawPolyMode::Fill))
            {
                glDrawElementsInstancedBaseVertexBaseInstance(
                    GL_TRIANGLES,
                    draw.indexCount,
                    GL_UNSIGNED_INT,
                    (void*)(sizeof(Vertex::Index) * draw.indexStart),
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
            glDisable(GL_POLYGON_OFFSET_FILL);
        }

        void OpenGLLSceneShadow::buildShader()
        {
            GLint result = GL_FALSE;

            // Vertex
            const std::vector<char> vertexShaderCode =
                FileSystem::read("Assets/Engine/Shaders/OpenGL/Scene/Shadow.overt");

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
            glTextureStorage2D(m_depthMapBuffer, 1, GL_DEPTH_COMPONENT32F, SHADOW_MAP_WIDTH, SHADOW_MAP_HEIGHT);

            glTextureParameteri(m_depthMapBuffer, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            glTextureParameteri(m_depthMapBuffer, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

            glTextureParameteri(m_depthMapBuffer, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
            glTextureParameteri(m_depthMapBuffer, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
            glTextureParameteri(m_depthMapBuffer, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_BORDER);

            float borderColor[] = {1.0f, 1.0f, 1.0f, 1.0f};
            glTextureParameterfv(m_depthMapBuffer, GL_TEXTURE_BORDER_COLOR, borderColor);

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