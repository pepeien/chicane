#include "Chicane/Renderer/Backend/OpenGL/Layer/UI.hpp"

#include <algorithm>
#include <cmath>

#include <glad/gl.h>

#include "Chicane/Core/FileSystem.hpp"

#include "Chicane/Renderer/Backend/OpenGL.hpp"

namespace Chicane
{
    namespace Renderer
    {
        OpenGLLUI::OpenGLLUI()
            : Layer(u_LAYER_ID),
              m_shaderProgram(0),
              m_primitiveVertexArray(0),
              m_primitiveVertexBuffer(0),
              m_primitiveIndexBuffer(0),
              m_instanceBuffer(0),
              m_glyphBuffer(0),
              m_backdropTexture(0),
              m_backdropFramebuffer(0),
              m_backdropWidth(0),
              m_backdropHeight(0)
        {}

        void OpenGLLUI::onInit()
        {
            buildShader();
            buildPrimitiveVertexArray();
            buildPrimitiveVertexBuffer();
            buildPrimitiveIndexBuffer();
            buildInstanceData();
            buildGlyphBuffer();
        }

        void OpenGLLUI::onDestruction()
        {
            destroyShader();
            destroyPrimitiveData();
            destroyInstanceData();
            destroyGlyphData();
            destroyBackdrop();
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
                glNamedBufferSubData(
                    m_glyphBuffer,
                    0,
                    sizeof(float) * inResource.getGlyphOutlines().size(),
                    inResource.getGlyphOutlines().data()
                );
            }
        }

        bool OpenGLLUI::onBeginRender(const Frame& inFrame)
        {
            if (!inFrame.hasDraws(DrawPolyType::e2D, DrawPolyMode::Fill))
            {
                return false;
            }

            return true;
        }

        void OpenGLLUI::onRender(const Frame& inFrame, void* inData)
        {
            glUseProgram(m_shaderProgram);

            glDisable(GL_DEPTH_TEST);
            glDepthMask(GL_FALSE);

            glDisable(GL_CULL_FACE);

            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

            glFrontFace(GL_CCW);

            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

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

            const std::uint32_t width  = std::max(1u, static_cast<std::uint32_t>(viewport.size.x));
            const std::uint32_t height = std::max(1u, static_cast<std::uint32_t>(viewport.size.y));

            ensureBackdrop(width, height);
            glBindTextureUnit(1, m_backdropTexture);

            const DrawPoly2DInstance::List& instances = inFrame.getInstances2D();

            auto drawRange = [&](const DrawPoly& draw, std::uint32_t first, std::uint32_t count)
            {
                if (count == 0)
                {
                    return;
                }

                glDrawElementsInstancedBaseVertexBaseInstance(
                    GL_TRIANGLES,
                    draw.indexCount,
                    GL_UNSIGNED_INT,
                    (void*)(sizeof(Vertex::Index) * draw.indexStart),
                    count,
                    draw.vertexStart,
                    first
                );
            };

            for (const DrawPoly& draw : inFrame.getDraws(DrawPolyType::e2D, DrawPolyMode::Fill))
            {
                std::uint32_t       runStart    = draw.instanceStart;
                std::uint32_t       runCount    = 0;
                bool                runBackdrop = false;
                const std::uint32_t instanceEnd = draw.instanceStart + draw.instanceCount;

                auto flush = [&]()
                {
                    if (runCount == 0)
                    {
                        return;
                    }

                    if (runBackdrop)
                    {
                        glMemoryBarrier(GL_FRAMEBUFFER_BARRIER_BIT);
                        copyBackdrop(viewport);
                        glBindTextureUnit(1, m_backdropTexture);
                        glViewport(viewport.position.x, viewport.position.y, viewport.size.x, viewport.size.y);
                    }

                    drawRange(draw, runStart, runCount);
                    runCount = 0;
                };

                for (std::uint32_t i = draw.instanceStart; i < instanceEnd; i++)
                {
                    const bool backdrop = i < instances.size() && instances.at(i).backdropBlur > 0.0f;

                    if (runCount == 0)
                    {
                        runStart    = i;
                        runBackdrop = backdrop;
                        runCount    = 1;

                        continue;
                    }

                    if (backdrop != runBackdrop || backdrop)
                    {
                        flush();
                        runStart    = i;
                        runBackdrop = backdrop;
                        runCount    = 1;

                        continue;
                    }

                    runCount++;
                }

                flush();
            }
        }

        void OpenGLLUI::onEndRender()
        {
            glDepthMask(GL_TRUE);
            glDisable(GL_BLEND);
        }

        void OpenGLLUI::buildShader()
        {
            GLint result = GL_FALSE;

            // Vertex
            const std::vector<char> vertexShaderCode = FileSystem::read("Assets/Engine/Shaders/OpenGL/UI.overt");

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
            const std::vector<char> fragmentShaderCode = FileSystem::read("Assets/Engine/Shaders/OpenGL/UI.ofrag");

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

        void OpenGLLUI::buildGlyphBuffer()
        {
            glCreateBuffers(1, &m_glyphBuffer);
            glNamedBufferData(m_glyphBuffer, m_backend->getResourceBudget(Resource::UIGlyphs), nullptr, GL_STATIC_DRAW);

            glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 4, m_glyphBuffer);
        }

        void OpenGLLUI::destroyGlyphData()
        {
            glDeleteBuffers(1, &m_glyphBuffer);
        }

        void OpenGLLUI::ensureBackdrop(std::uint32_t inWidth, std::uint32_t inHeight)
        {
            if (inWidth == 0 || inHeight == 0)
            {
                return;
            }

            if (m_backdropTexture != 0 && m_backdropWidth == inWidth && m_backdropHeight == inHeight)
            {
                return;
            }

            destroyBackdrop();

            const std::int32_t levels =
                1 + static_cast<std::int32_t>(std::floor(std::log2(static_cast<float>(std::max(inWidth, inHeight)))));

            glCreateTextures(GL_TEXTURE_2D, 1, &m_backdropTexture);
            glTextureStorage2D(m_backdropTexture, levels, GL_RGBA8, inWidth, inHeight);
            glTextureParameteri(m_backdropTexture, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
            glTextureParameteri(m_backdropTexture, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTextureParameteri(m_backdropTexture, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTextureParameteri(m_backdropTexture, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            glTextureParameteri(m_backdropTexture, GL_TEXTURE_MAX_LEVEL, levels - 1);

            glCreateFramebuffers(1, &m_backdropFramebuffer);
            glNamedFramebufferTexture(m_backdropFramebuffer, GL_COLOR_ATTACHMENT0, m_backdropTexture, 0);

            m_backdropWidth  = inWidth;
            m_backdropHeight = inHeight;
        }

        void OpenGLLUI::copyBackdrop(const Viewport& inViewport)
        {
            if (m_backdropTexture == 0 || m_backdropFramebuffer == 0)
            {
                return;
            }

            const std::int32_t x      = static_cast<std::int32_t>(inViewport.position.x);
            const std::int32_t y      = static_cast<std::int32_t>(inViewport.position.y);
            const std::int32_t width  = static_cast<std::int32_t>(inViewport.size.x);
            const std::int32_t height = static_cast<std::int32_t>(inViewport.size.y);

            GLint source = 0;
            glGetIntegerv(GL_DRAW_FRAMEBUFFER_BINDING, &source);

            glBindTextureUnit(1, 0);

            glBindFramebuffer(GL_READ_FRAMEBUFFER, source);
            glReadBuffer(source == 0 ? GL_BACK : GL_COLOR_ATTACHMENT0);
            glBindFramebuffer(GL_DRAW_FRAMEBUFFER, static_cast<GLuint>(m_backdropFramebuffer));
            glDrawBuffer(GL_COLOR_ATTACHMENT0);
            glBlitFramebuffer(x, y, x + width, y + height, 0, 0, width, height, GL_COLOR_BUFFER_BIT, GL_NEAREST);

            glBindFramebuffer(GL_FRAMEBUFFER, source);
            glGenerateTextureMipmap(m_backdropTexture);
            if (source == 0)
            {
                glReadBuffer(GL_BACK);
                glDrawBuffer(GL_BACK);
            }
        }

        void OpenGLLUI::destroyBackdrop()
        {
            if (m_backdropFramebuffer != 0)
            {
                glDeleteFramebuffers(1, &m_backdropFramebuffer);
                m_backdropFramebuffer = 0;
            }

            if (m_backdropTexture == 0)
            {
                return;
            }

            glDeleteTextures(1, &m_backdropTexture);

            m_backdropTexture = 0;
            m_backdropWidth   = 0;
            m_backdropHeight  = 0;
        }
    }
}