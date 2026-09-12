#include "Chicane/Renderer/Backend/OpenGL/Layer/Scene/Line.hpp"

#include <cstddef>

#include <glad/gl.h>

#include "Chicane/Renderer.hpp"
#include "Chicane/Renderer/Backend/OpenGL.hpp"
#include "Chicane/Renderer/Debug/Mode.hpp"
#include "Chicane/Renderer/Draw/Poly.hpp"
#include "Chicane/Renderer/Draw/Poly/3D/Flag.hpp"
#include "Chicane/Renderer/Draw/Poly/Topology.hpp"
#include "Chicane/Renderer/Instance.hpp"

namespace Chicane
{
    namespace Renderer
    {
        OpenGLLSceneLine::OpenGLLSceneLine()
            : Layer(SCENE_LINE_LAYER_ID),
              m_meshShaderProgram(0),
              m_outlineShaderProgram(0),
              m_immediateVertexArray(0),
              m_immediateVertexBuffer(0),
              m_immediateIndexBuffer(0),
              m_immediateVertexCapacity(0),
              m_immediateIndexCapacity(0)
        {}

        void OpenGLLSceneLine::onInit()
        {
            buildMeshShader();
            buildOutlineShader();
            buildImmediateVertexArray();
        }

        void OpenGLLSceneLine::onDestruction()
        {
            destroyImmediateVertexArray();
            destroyOutlineShader();
            destroyMeshShader();
        }

        bool OpenGLLSceneLine::shouldDrawMeshWireframe(const Frame& inFrame) const
        {
            const Instance* renderer = getBackend()->getRenderer();
            if (inFrame.hasDraws(DrawPolyType::e3D, DrawPolyMode::Fill) &&
                (renderer->hasDebug(DebugMode::Meshes) || !renderer->hasFill()))
            {
                return true;
            }

            for (const DrawPoly& draw : inFrame.getDraws(DrawPolyType::e3D, DrawPolyMode::Line))
            {
                if (!draw.isLineList())
                {
                    return true;
                }
            }

            return false;
        }

        bool OpenGLLSceneLine::shouldDrawLineList(const Frame& inFrame) const
        {
            for (const DrawPoly& draw : inFrame.getDraws(DrawPolyType::e3D, DrawPolyMode::Line))
            {
                if (draw.isLineList())
                {
                    return true;
                }
            }

            return false;
        }

        bool OpenGLLSceneLine::shouldDrawOutline(const Frame& inFrame) const
        {
            return inFrame.hasOutlineDraws();
        }

        bool OpenGLLSceneLine::onBeginRender(const Frame& inFrame)
        {
            return shouldDrawMeshWireframe(inFrame) || shouldDrawLineList(inFrame) || shouldDrawOutline(inFrame);
        }

        void OpenGLLSceneLine::onRender(const Frame& inFrame, void* inData)
        {
            OpenGLBackend*  backend  = getBackend<OpenGLBackend>();
            const Instance* renderer = backend->getRenderer();
            OpenGLFrame&    frame    = *((OpenGLFrame*)inData);

            backend->useViewport(this);

            Depth depth;
            depth.bCanWrite = true;
            depth.compare   = DepthCompare::LessOrEqual;
            backend->enableDepth(depth);
            backend->disableCulling();

            if (shouldDrawMeshWireframe(inFrame))
            {
                backend->bindVertexArray(frame.getObject(SCENE_LAYER_ID));
                backend->useProgram(m_meshShaderProgram);
                glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

                auto drawBatch = [&](DrawPolyMode inMode)
                {
                    for (const DrawPoly& draw : inFrame.getDraws(DrawPolyType::e3D, inMode))
                    {
                        if (draw.isLineList())
                        {
                            continue;
                        }

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
                };

                if (inFrame.hasDraws(DrawPolyType::e3D, DrawPolyMode::Line))
                {
                    drawBatch(DrawPolyMode::Line);
                }

                if (renderer->hasDebug(DebugMode::Meshes) || !renderer->hasFill())
                {
                    for (const DrawPoly& draw : inFrame.getSceneDraws())
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
                }

                glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
            }

            if (shouldDrawOutline(inFrame))
            {
                Depth outlineDepth;
                outlineDepth.bCanWrite = false;
                outlineDepth.compare   = DepthCompare::LessOrEqual;
                backend->enableDepth(outlineDepth);
                backend->disableCulling();

                backend->bindVertexArray(frame.getObject(SCENE_LAYER_ID));
                backend->useProgram(m_outlineShaderProgram);
                glProgramUniform4f(
                    m_outlineShaderProgram,
                    1,
                    static_cast<float>(OUTLINE_COLOR.r) / 255.0f,
                    static_cast<float>(OUTLINE_COLOR.g) / 255.0f,
                    static_cast<float>(OUTLINE_COLOR.b) / 255.0f,
                    static_cast<float>(OUTLINE_COLOR.a) / 255.0f
                );
                glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

                glEnable(GL_STENCIL_TEST);
                glClear(GL_STENCIL_BUFFER_BIT);

                glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
                glStencilMask(0xFF);
                glStencilFunc(GL_ALWAYS, 1, 0xFF);
                glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
                drawOutlineMeshes(inFrame, 0.0f, 0.0f);

                const Viewport viewport = backend->getGLViewport(this);
                const float    scaleX   = viewport.size.x > 0.0f ? 4.0f / viewport.size.x : 0.0f;
                const float    scaleY   = viewport.size.y > 0.0f ? 4.0f / viewport.size.y : 0.0f;

                glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
                glStencilMask(0x00);
                glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
                glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);

                static const float kOffsets[8][2] = {
                    {1.0f,         0.0f        },
                    {-1.0f,        0.0f        },
                    {0.0f,         1.0f        },
                    {0.0f,         -1.0f       },
                    {0.70710678f,  0.70710678f },
                    {0.70710678f,  -0.70710678f},
                    {-0.70710678f, 0.70710678f },
                    {-0.70710678f, -0.70710678f}
                };

                for (const float* offset : kOffsets)
                {
                    drawOutlineMeshes(inFrame, offset[0] * scaleX, offset[1] * scaleY);
                }

                glDisable(GL_STENCIL_TEST);
                glStencilMask(0xFF);

                outlineDepth.bCanWrite = true;
                backend->enableDepth(outlineDepth);
            }

            if (!shouldDrawLineList(inFrame))
            {
                return;
            }

            backend->enableDepth(depth);
            backend->disableCulling();
            backend->useProgram(m_meshShaderProgram);
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
            glLineWidth(1.0f);

            if (!inFrame.hasImmediateVertices())
            {
                return;
            }

            uploadImmediateGeometry(inFrame);
            glBindVertexArray(m_immediateVertexArray);
            for (const DrawPoly& draw : inFrame.getDraws(DrawPolyType::e3D, DrawPolyMode::Line))
            {
                if (!draw.isLineList())
                {
                    continue;
                }

                if (inFrame.isForegroundDraw(draw))
                {
                    backend->disableDepth();
                }
                else
                {
                    backend->enableDepth(depth);
                }

                drawLineList(draw);
            }
        }

        void OpenGLLSceneLine::onEndRender()
        {
            OpenGLBackend* backend = getBackend<OpenGLBackend>();
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
            backend->disableDepth();
            glDisable(GL_STENCIL_TEST);
            glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
            glStencilMask(0xFF);
        }

        void OpenGLLSceneLine::buildMeshShader()
        {
            Shader::List shaders;

            Shader vertex;
            vertex.type   = ShaderType::Vertex;
            vertex.source = "Assets/Engine/Shaders/OpenGL/Scene/Line.overt";
            shaders.push_back(vertex);

            Shader fragment;
            fragment.type   = ShaderType::Fragment;
            fragment.source = "Assets/Engine/Shaders/OpenGL/Scene/Line.ofrag";
            shaders.push_back(fragment);

            m_meshShaderProgram = getBackend<OpenGLBackend>()->initShader(shaders);
        }

        void OpenGLLSceneLine::destroyMeshShader()
        {
            if (m_meshShaderProgram)
            {
                getBackend<OpenGLBackend>()->destroyProgram(m_meshShaderProgram);
                m_meshShaderProgram = 0;
            }
        }

        void OpenGLLSceneLine::buildOutlineShader()
        {
            Shader::List shaders;

            Shader vertex;
            vertex.type   = ShaderType::Vertex;
            vertex.source = "Assets/Engine/Shaders/OpenGL/Scene/Outline.overt";
            shaders.push_back(vertex);

            Shader fragment;
            fragment.type   = ShaderType::Fragment;
            fragment.source = "Assets/Engine/Shaders/OpenGL/Scene/Outline.ofrag";
            shaders.push_back(fragment);

            m_outlineShaderProgram = getBackend<OpenGLBackend>()->initShader(shaders);
        }

        void OpenGLLSceneLine::destroyOutlineShader()
        {
            if (m_outlineShaderProgram)
            {
                getBackend<OpenGLBackend>()->destroyProgram(m_outlineShaderProgram);
                m_outlineShaderProgram = 0;
            }
        }

        void OpenGLLSceneLine::buildImmediateVertexArray()
        {
            OpenGLBackend* backend = getBackend<OpenGLBackend>();

            m_immediateVertexArray = backend->initVertexArray(1);
            glCreateBuffers(1, &m_immediateVertexBuffer);
            glCreateBuffers(1, &m_immediateIndexBuffer);

            glEnableVertexArrayAttrib(m_immediateVertexArray, 0);
            glVertexArrayAttribFormat(m_immediateVertexArray, 0, 3, GL_FLOAT, GL_FALSE, offsetof(Vertex, position));
            glVertexArrayAttribBinding(m_immediateVertexArray, 0, 0);

            glEnableVertexArrayAttrib(m_immediateVertexArray, 1);
            glVertexArrayAttribFormat(m_immediateVertexArray, 1, 4, GL_FLOAT, GL_FALSE, offsetof(Vertex, color));
            glVertexArrayAttribBinding(m_immediateVertexArray, 1, 0);

            glEnableVertexArrayAttrib(m_immediateVertexArray, 2);
            glVertexArrayAttribFormat(m_immediateVertexArray, 2, 2, GL_FLOAT, GL_FALSE, offsetof(Vertex, uv));
            glVertexArrayAttribBinding(m_immediateVertexArray, 2, 0);

            glEnableVertexArrayAttrib(m_immediateVertexArray, 3);
            glVertexArrayAttribFormat(m_immediateVertexArray, 3, 3, GL_FLOAT, GL_FALSE, offsetof(Vertex, normal));
            glVertexArrayAttribBinding(m_immediateVertexArray, 3, 0);
        }

        void OpenGLLSceneLine::destroyImmediateVertexArray()
        {
            if (m_immediateIndexBuffer)
            {
                glDeleteBuffers(1, &m_immediateIndexBuffer);
                m_immediateIndexBuffer = 0;
            }

            if (m_immediateVertexBuffer)
            {
                glDeleteBuffers(1, &m_immediateVertexBuffer);
                m_immediateVertexBuffer = 0;
            }

            if (m_immediateVertexArray)
            {
                getBackend<OpenGLBackend>()->destroyVertexArray(m_immediateVertexArray);
                m_immediateVertexArray = 0;
            }

            m_immediateVertexCapacity = 0;
            m_immediateIndexCapacity  = 0;
        }

        void OpenGLLSceneLine::uploadImmediateGeometry(const Frame& inFrame)
        {
            const Vertex::List& vertices    = inFrame.getImmediateVertices();
            const std::size_t   vertexCount = vertices.size();
            const GLsizeiptr    vertexSize  = static_cast<GLsizeiptr>(sizeof(Vertex) * vertexCount);

            if (vertexCount > m_immediateVertexCapacity)
            {
                glNamedBufferData(m_immediateVertexBuffer, vertexSize, nullptr, GL_DYNAMIC_DRAW);
                m_immediateVertexCapacity = vertexCount;
            }

            if (!vertices.empty())
            {
                glNamedBufferSubData(m_immediateVertexBuffer, 0, vertexSize, vertices.data());
            }

            glVertexArrayVertexBuffer(m_immediateVertexArray, 0, m_immediateVertexBuffer, 0, sizeof(Vertex));

            const Vertex::Indices& indices    = inFrame.getImmediateIndices();
            const std::size_t      indexCount = indices.size();
            const GLsizeiptr       indexSize  = static_cast<GLsizeiptr>(sizeof(Vertex::Index) * indexCount);

            if (indexCount > m_immediateIndexCapacity)
            {
                glNamedBufferData(m_immediateIndexBuffer, indexSize, nullptr, GL_DYNAMIC_DRAW);
                m_immediateIndexCapacity = indexCount;
            }

            if (!indices.empty())
            {
                glNamedBufferSubData(m_immediateIndexBuffer, 0, indexSize, indices.data());
            }

            glVertexArrayElementBuffer(m_immediateVertexArray, m_immediateIndexBuffer);
        }

        void OpenGLLSceneLine::drawOutlineMeshes(const Frame& inFrame, float inOffsetX, float inOffsetY) const
        {
            glProgramUniform2f(m_outlineShaderProgram, 0, inOffsetX, inOffsetY);

            for (const DrawPoly& draw : inFrame.getOutlineDraws())
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
        }

        void OpenGLLSceneLine::drawLineList(const DrawPoly& inDraw) const
        {
            if (inDraw.indexCount == 0)
            {
                glDrawArraysInstancedBaseInstance(
                    GL_LINES,
                    static_cast<GLint>(inDraw.vertexStart),
                    static_cast<GLsizei>(inDraw.vertexCount),
                    static_cast<GLsizei>(inDraw.instanceCount),
                    inDraw.instanceStart
                );

                return;
            }

            glDrawElementsInstancedBaseVertexBaseInstance(
                GL_LINES,
                inDraw.indexCount,
                GL_UNSIGNED_INT,
                (void*)(sizeof(Vertex::Index) * inDraw.indexStart),
                inDraw.instanceCount,
                inDraw.vertexStart,
                inDraw.instanceStart
            );
        }
    }
}
