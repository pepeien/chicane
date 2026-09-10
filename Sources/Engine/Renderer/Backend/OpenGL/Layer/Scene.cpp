#include "Chicane/Renderer/Backend/OpenGL/Layer/Scene.hpp"

#include <glad/gl.h>

#include "Chicane/Renderer/Backend/OpenGL.hpp"
#include "Chicane/Renderer/Backend/OpenGL/Layer/Scene/Foreground.hpp"
#include "Chicane/Renderer/Backend/OpenGL/Layer/Scene/Line.hpp"
#include "Chicane/Renderer/Backend/OpenGL/Layer/Scene/Mesh.hpp"
#include "Chicane/Renderer/Backend/OpenGL/Layer/Scene/Shadow.hpp"
#include "Chicane/Renderer/Backend/OpenGL/Layer/Scene/Sky.hpp"
#include "Chicane/Renderer/Shadow.hpp"

namespace Chicane
{
    namespace Renderer
    {
        OpenGLLScene::OpenGLLScene()
            : Layer(SCENE_LAYER_ID)
        {}

        void OpenGLLScene::onInit()
        {
            buildModelVertexArray();
            buildModelVertexBuffer();
            buildModelIndexBuffer();
            buildInstanceData();
            buildLayers();
        }

        void OpenGLLScene::onDestruction()
        {
            destroyModelData();
            destroyInstanceData();
        }

        void OpenGLLScene::onLoad(DrawPolyType inType, const DrawPolyResource& inResource)
        {
            if (inType == DrawPolyType::e3D)
            {
                glNamedBufferSubData(
                    m_modelVertexBuffer,
                    0,
                    sizeof(Vertex) * inResource.getVertices().size(),
                    inResource.getVertices().data()
                );
                glNamedBufferSubData(
                    m_modelIndexBuffer,
                    0,
                    sizeof(Vertex::Index) * inResource.getIndices().size(),
                    inResource.getIndices().data()
                );
            }
        }

        bool OpenGLLScene::onBeginRender(const Frame& inFrame)
        {
            const DrawPoly3DInstance::List& instances = inFrame.getInstances3D();
            if (instances.empty() && inFrame.getLights().empty())
            {
                return false;
            }

            return true;
        }

        void OpenGLLScene::onRender(const Frame& inFrame, void* inData)
        {
            glBindVertexArray(m_modelVertexArray);
            glVertexArrayElementBuffer(m_modelVertexArray, m_modelIndexBuffer);
            glVertexArrayVertexBuffer(m_modelVertexArray, 0, m_modelVertexBuffer, 0, sizeof(Vertex));

            View camera = inFrame.getCamera();
            camera.depthZeroToOne();

            m_light = Shadow::build(inFrame.getCamera(), inFrame.getLights());
            for (std::uint32_t cascade = 0; cascade < SHADOW_CASCADE_COUNT; ++cascade)
            {
                Mat4 depth                   = Mat4::One;
                depth[2][2]                  = 0.5f;
                depth[3][2]                  = 0.5f;
                m_light.projections[cascade] = depth * m_light.projections[cascade];
            }

            std::size_t size   = sizeof(View);
            std::size_t offset = 0;
            glNamedBufferSubData(m_instanceBuffer, offset, size, &camera);
            offset += size;

            size = sizeof(ShadowLight);
            glNamedBufferSubData(m_instanceBuffer, offset, size, &m_light);
            offset += size;

            const DrawPoly3DInstance::List& instances = inFrame.getInstances3D();
            size                                      = sizeof(DrawPoly3DInstance) * instances.size();
            glNamedBufferSubData(m_instanceBuffer, offset, size, instances.data());
        }

        void OpenGLLScene::buildModelVertexArray()
        {
            glCreateVertexArrays(1, &m_modelVertexArray);
            for (OpenGLFrame& frame : getBackend<OpenGLBackend>()->frames)
            {
                frame.addObject(m_id, m_modelVertexArray);
            }
        }

        void OpenGLLScene::buildModelVertexBuffer()
        {
            glCreateBuffers(1, &m_modelVertexBuffer);
            glNamedBufferData(
                m_modelVertexBuffer,
                m_backend->getResourceBudget(Resource::SceneVertices),
                nullptr,
                GL_DYNAMIC_DRAW
            );

            // Position
            glEnableVertexArrayAttrib(m_modelVertexArray, 0);
            glVertexArrayAttribFormat(m_modelVertexArray, 0, 3, GL_FLOAT, GL_FALSE, offsetof(Vertex, position));
            glVertexArrayAttribBinding(m_modelVertexArray, 0, 0);

            // Color
            glEnableVertexArrayAttrib(m_modelVertexArray, 1);
            glVertexArrayAttribFormat(m_modelVertexArray, 1, 4, GL_FLOAT, GL_FALSE, offsetof(Vertex, color));
            glVertexArrayAttribBinding(m_modelVertexArray, 1, 0);

            // UV
            glEnableVertexArrayAttrib(m_modelVertexArray, 2);
            glVertexArrayAttribFormat(m_modelVertexArray, 2, 2, GL_FLOAT, GL_FALSE, offsetof(Vertex, uv));
            glVertexArrayAttribBinding(m_modelVertexArray, 2, 0);

            // Normal
            glEnableVertexArrayAttrib(m_modelVertexArray, 3);
            glVertexArrayAttribFormat(m_modelVertexArray, 3, 3, GL_FLOAT, GL_FALSE, offsetof(Vertex, normal));
            glVertexArrayAttribBinding(m_modelVertexArray, 3, 0);
        }

        void OpenGLLScene::buildModelIndexBuffer()
        {
            glCreateBuffers(1, &m_modelIndexBuffer);
            glNamedBufferData(
                m_modelIndexBuffer,
                m_backend->getResourceBudget(Resource::SceneIndices),
                nullptr,
                GL_DYNAMIC_DRAW
            );
        }

        void OpenGLLScene::destroyModelData()
        {
            for (OpenGLFrame& frame : getBackend<OpenGLBackend>()->frames)
            {
                frame.removeObject(m_id);
            }
            glDeleteVertexArrays(1, &m_modelVertexArray);
            glDeleteBuffers(1, &m_modelVertexBuffer);
            glDeleteBuffers(1, &m_modelIndexBuffer);
        }

        void OpenGLLScene::buildInstanceData()
        {
            glCreateBuffers(1, &m_instanceBuffer);
            glNamedBufferData(
                m_instanceBuffer,
                m_backend->getResourceBudget(Resource::SceneCamera) +
                    m_backend->getResourceBudget(Resource::SceneLights) +
                    m_backend->getResourceBudget(Resource::SceneInstances),
                nullptr,
                GL_DYNAMIC_DRAW
            );

            glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, m_instanceBuffer);
        }

        void OpenGLLScene::destroyInstanceData()
        {
            glDeleteBuffers(1, &m_instanceBuffer);
        }

        void OpenGLLScene::buildLayers()
        {
            ListPush<Layer*> settings;
            settings.strategy = ListPushStrategy::Back;

            m_backend->addLayer<OpenGLLSceneSky>(settings);
            m_backend->addLayer<OpenGLLSceneShadow>(settings);
            m_backend->addLayer<OpenGLLSceneMesh>(settings);
            m_backend->addLayer<OpenGLLSceneLine>(settings);
            m_backend->addLayer<OpenGLLSceneForeground>(settings);
        }
    }
}
