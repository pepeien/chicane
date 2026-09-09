#include "Chicane/Renderer/Backend/OpenGL/Layer/Scene/Line.hpp"

#include <glad/gl.h>

#include "Chicane/Renderer.hpp"
#include "Chicane/Renderer/Backend/OpenGL.hpp"
#include "Chicane/Renderer/Debug/Mode.hpp"
#include "Chicane/Renderer/Instance.hpp"

namespace Chicane
{
    namespace Renderer
    {
        OpenGLLSceneLine::OpenGLLSceneLine()
            : Layer(SCENE_LINE_LAYER_ID),
              m_meshShaderProgram(0),
              m_overlayShaderProgram(0),
              m_overlayVertexArray(0),
              m_overlayVertexBuffer(0),
              m_overlayVertexCount(0)
        {}

        void OpenGLLSceneLine::onInit()
        {
            buildMeshShader();
            buildOverlayShader();
            buildOverlayVertexArray();
        }

        void OpenGLLSceneLine::onDestruction()
        {
            destroyOverlayVertexArray();
            destroyOverlayShader();
            destroyMeshShader();
        }

        bool OpenGLLSceneLine::shouldDrawMeshWireframe(const Frame& inFrame) const
        {
            if (inFrame.hasDraws(DrawPolyType::e3D, DrawPolyMode::Line))
            {
                return true;
            }

            const Instance* renderer = getBackend()->getRenderer();

            return renderer->hasDebug(DebugMode::Meshes) && inFrame.hasDraws(DrawPolyType::e3D, DrawPolyMode::Fill);
        }

        bool OpenGLLSceneLine::shouldDrawOverlay() const
        {
            const Instance* renderer = getBackend()->getRenderer();

            return (renderer->hasDebug(DebugMode::Bounds) || renderer->hasDebug(DebugMode::Traces) ||
                    renderer->hasDebug(DebugMode::Colliders)) &&
                   renderer->hasDebugOverlay();
        }

        bool OpenGLLSceneLine::onBeginRender(const Frame& inFrame)
        {
            return shouldDrawMeshWireframe(inFrame) || shouldDrawOverlay();
        }

        void OpenGLLSceneLine::onRender(const Frame& inFrame, void* inData)
        {
            OpenGLBackend*  backend  = getBackend<OpenGLBackend>();
            const Instance* renderer = backend->getRenderer();

            backend->useViewport(this);

            Depth depth;
            depth.bCanWrite = true;
            depth.compare   = DepthCompare::LessOrEqual;
            backend->enableDepth(depth);
            backend->disableCulling();

            if (shouldDrawMeshWireframe(inFrame))
            {
                backend->useProgram(m_meshShaderProgram);
                glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

                auto drawBatch = [&](DrawPolyMode inMode)
                {
                    for (const DrawPoly& draw : inFrame.getDraws(DrawPolyType::e3D, inMode))
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
                };

                if (inFrame.hasDraws(DrawPolyType::e3D, DrawPolyMode::Line))
                {
                    drawBatch(DrawPolyMode::Line);
                }

                if (renderer->hasDebug(DebugMode::Meshes))
                {
                    drawBatch(DrawPolyMode::Fill);
                }

                glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
            }

            if (!shouldDrawOverlay())
            {
                return;
            }

            Vertex::List vertices = renderer->getDebugOverlayVertices();
            if (vertices.empty())
            {
                return;
            }

            uploadOverlayBuffer(vertices);

            backend->useProgram(m_overlayShaderProgram);

            DrawPoly draw;
            draw.topology   = DrawPolyTopology::LineList;
            draw.indexCount = m_overlayVertexCount;
            draw.indexStart = 0U;
            backend->drawPolyArrays(draw, m_overlayVertexArray);
        }

        void OpenGLLSceneLine::onEndRender()
        {
            OpenGLBackend* backend = getBackend<OpenGLBackend>();
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
            backend->disableDepth();
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

        void OpenGLLSceneLine::buildOverlayShader()
        {
            Shader::List shaders;

            Shader vertex;
            vertex.type   = ShaderType::Vertex;
            vertex.source = "Assets/Engine/Shaders/OpenGL/Scene/Line/Overlay.overt";
            shaders.push_back(vertex);

            Shader fragment;
            fragment.type   = ShaderType::Fragment;
            fragment.source = "Assets/Engine/Shaders/OpenGL/Scene/Line/Overlay.ofrag";
            shaders.push_back(fragment);

            m_overlayShaderProgram = getBackend<OpenGLBackend>()->initShader(shaders);
        }

        void OpenGLLSceneLine::destroyOverlayShader()
        {
            if (m_overlayShaderProgram)
            {
                getBackend<OpenGLBackend>()->destroyProgram(m_overlayShaderProgram);
                m_overlayShaderProgram = 0;
            }
        }

        void OpenGLLSceneLine::buildOverlayVertexArray()
        {
            OpenGLBackend* backend = getBackend<OpenGLBackend>();

            m_overlayVertexArray = backend->initVertexArray(1);
            glCreateBuffers(1, &m_overlayVertexBuffer);

            glEnableVertexArrayAttrib(m_overlayVertexArray, 0);
            glVertexArrayAttribFormat(m_overlayVertexArray, 0, 3, GL_FLOAT, GL_FALSE, offsetof(Vertex, position));
            glVertexArrayAttribBinding(m_overlayVertexArray, 0, 0);

            glEnableVertexArrayAttrib(m_overlayVertexArray, 1);
            glVertexArrayAttribFormat(m_overlayVertexArray, 1, 4, GL_FLOAT, GL_FALSE, offsetof(Vertex, color));
            glVertexArrayAttribBinding(m_overlayVertexArray, 1, 0);

            glEnableVertexArrayAttrib(m_overlayVertexArray, 2);
            glVertexArrayAttribFormat(m_overlayVertexArray, 2, 2, GL_FLOAT, GL_FALSE, offsetof(Vertex, uv));
            glVertexArrayAttribBinding(m_overlayVertexArray, 2, 0);

            glEnableVertexArrayAttrib(m_overlayVertexArray, 3);
            glVertexArrayAttribFormat(m_overlayVertexArray, 3, 3, GL_FLOAT, GL_FALSE, offsetof(Vertex, normal));
            glVertexArrayAttribBinding(m_overlayVertexArray, 3, 0);
        }

        void OpenGLLSceneLine::destroyOverlayVertexArray()
        {
            if (m_overlayVertexBuffer)
            {
                glDeleteBuffers(1, &m_overlayVertexBuffer);
                m_overlayVertexBuffer = 0;
            }

            if (m_overlayVertexArray)
            {
                getBackend<OpenGLBackend>()->destroyVertexArray(m_overlayVertexArray);
                m_overlayVertexArray = 0;
            }

            m_overlayVertexCount = 0;
        }

        void OpenGLLSceneLine::uploadOverlayBuffer(const Vertex::List& inVertices)
        {
            const GLsizeiptr size = static_cast<GLsizeiptr>(sizeof(Vertex) * inVertices.size());

            glNamedBufferData(m_overlayVertexBuffer, size, inVertices.data(), GL_DYNAMIC_DRAW);
            glVertexArrayVertexBuffer(m_overlayVertexArray, 0, m_overlayVertexBuffer, 0, sizeof(Vertex));

            m_overlayVertexCount = static_cast<std::uint32_t>(inVertices.size());
        }
    }
}
