#include "Chicane/Renderer/Backend/OpenGL/Layer/Scene/Particle.hpp"

#include <algorithm>

#include <glad/gl.h>

#include "Chicane/Renderer/Backend/OpenGL.hpp"
#include "Chicane/Renderer/Draw/Particle.hpp"
#include "Chicane/Renderer/Shader.hpp"

namespace Chicane
{
    namespace Renderer
    {
        OpenGLLSceneParticle::OpenGLLSceneParticle()
            : Layer(SCENE_PARTICLE_LAYER_ID),
              m_shaderProgram(0),
              m_vertexArray(0),
              m_particleBuffer(0)
        {}

        void OpenGLLSceneParticle::onInit()
        {
            buildShader();
            buildBuffers();
        }

        void OpenGLLSceneParticle::onDestruction()
        {
            destroyShader();
            destroyBuffers();
        }

        bool OpenGLLSceneParticle::onBeginRender(const Frame& inFrame)
        {
            return inFrame.hasParticles();
        }

        void OpenGLLSceneParticle::onRender(const Frame& inFrame, void*)
        {
            const DrawParticle::List& particles = inFrame.getParticles();
            const std::uint32_t       count     = std::min(static_cast<std::uint32_t>(particles.size()), MAX_PARTICLES);

            glNamedBufferSubData(m_particleBuffer, 0, sizeof(DrawParticle) * count, particles.data());
            glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 4, m_particleBuffer);

            glUseProgram(m_shaderProgram);
            glBindVertexArray(m_vertexArray);

            glEnable(GL_DEPTH_TEST);
            glDepthFunc(GL_LESS);
            glDepthMask(GL_FALSE);

            glEnable(GL_BLEND);
            glBlendFunc(GL_ONE, GL_ONE);
            glDisable(GL_CULL_FACE);

            Viewport viewport = getBackend<OpenGLBackend>()->getGLViewport(this);
            glViewport(viewport.position.x, viewport.position.y, viewport.size.x, viewport.size.y);

            glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 4, static_cast<GLsizei>(count));

            glDepthMask(GL_TRUE);
        }

        void OpenGLLSceneParticle::onEndRender()
        {
            glDisable(GL_BLEND);
            glDisable(GL_DEPTH_TEST);
            glBindVertexArray(0);
        }

        void OpenGLLSceneParticle::buildShader()
        {
            Shader::List shaders;
            shaders.push_back({"Assets/Engine/Shaders/OpenGL/Scene/Particle.overt", ShaderType::Vertex});
            shaders.push_back({"Assets/Engine/Shaders/OpenGL/Scene/Particle.ofrag", ShaderType::Fragment});

            m_shaderProgram = getBackend<OpenGLBackend>()->initShader(shaders);
        }

        void OpenGLLSceneParticle::destroyShader()
        {
            if (m_shaderProgram != 0)
            {
                glDeleteProgram(m_shaderProgram);
                m_shaderProgram = 0;
            }
        }

        void OpenGLLSceneParticle::buildBuffers()
        {
            glCreateVertexArrays(1, &m_vertexArray);
            glCreateBuffers(1, &m_particleBuffer);
            glNamedBufferData(m_particleBuffer, sizeof(DrawParticle) * MAX_PARTICLES, nullptr, GL_DYNAMIC_DRAW);
        }

        void OpenGLLSceneParticle::destroyBuffers()
        {
            if (m_particleBuffer != 0)
            {
                glDeleteBuffers(1, &m_particleBuffer);
                m_particleBuffer = 0;
            }

            if (m_vertexArray != 0)
            {
                glDeleteVertexArrays(1, &m_vertexArray);
                m_vertexArray = 0;
            }
        }
    }
}
