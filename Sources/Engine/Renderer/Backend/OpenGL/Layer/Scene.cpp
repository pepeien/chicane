#include "Chicane/Renderer/Backend/OpenGL/Layer/Scene.hpp"

#include <GL/glew.h>

#include "Chicane/Renderer/Backend/OpenGL.hpp"
#include "Chicane/Renderer/Backend/OpenGL/Layer/Scene/Mesh.hpp"
#include "Chicane/Renderer/Backend/OpenGL/Layer/Scene/Shadow.hpp"
#include "Chicane/Renderer/Backend/OpenGL/Layer/Scene/Sky.hpp"

namespace Chicane
{
    namespace Renderer
    {
        OpenGLLScene::OpenGLLScene()
            : Layer(ID)
        {}

        void OpenGLLScene::onInit()
        {
            buildCameraData();
            buildLightData();
            buildModelVertexArray();
            buildModelVertexBuffer();
            buildModelIndexBuffer();
            buildInstanceData();
            buildLayers();
        }

        void OpenGLLScene::onDestruction()
        {
            destroyCameraData();
            destroyLightData();
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
            if (inFrame.get3DDraws().empty() && inFrame.getInstances3D().empty() && inFrame.getLights().empty())
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

            glBindBuffer(GL_UNIFORM_BUFFER, m_cameraBuffer);
            glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(View), &inFrame.getCamera());

            glBindBuffer(GL_UNIFORM_BUFFER, m_lightBuffer);
            glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(View), inFrame.getLights().data());

            glNamedBufferSubData(
                m_instanceBuffer,
                0,
                sizeof(DrawPoly3DInstance) * inFrame.getInstances3D().size(),
                inFrame.getInstances3D().data()
            );
        }

        void OpenGLLScene::buildCameraData()
        {
            glCreateBuffers(1, &m_cameraBuffer);
            glNamedBufferData(m_cameraBuffer, sizeof(View), nullptr, GL_DYNAMIC_DRAW);

            glBindBufferBase(GL_UNIFORM_BUFFER, 0, m_cameraBuffer);
        }

        void OpenGLLScene::destroyCameraData()
        {
            glDeleteBuffers(1, &m_cameraBuffer);
        }

        void OpenGLLScene::buildLightData()
        {
            glCreateBuffers(1, &m_lightBuffer);
            glNamedBufferData(m_lightBuffer, sizeof(View), nullptr, GL_DYNAMIC_DRAW);

            glBindBufferBase(GL_UNIFORM_BUFFER, 1, m_lightBuffer);
        }

        void OpenGLLScene::destroyLightData()
        {
            glDeleteBuffers(1, &m_lightBuffer);
        }

        void OpenGLLScene::buildModelVertexArray()
        {
            glCreateVertexArrays(1, &m_modelVertexArray);
        }

        void OpenGLLScene::buildModelVertexBuffer()
        {
            glCreateBuffers(1, &m_modelVertexBuffer);
            glNamedBufferData(
                m_modelVertexBuffer,
                m_backend->getResourceBudget(BackendResource::SceneVertices),
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
                m_backend->getResourceBudget(BackendResource::SceneIndices),
                nullptr,
                GL_DYNAMIC_DRAW
            );
        }

        void OpenGLLScene::destroyModelData()
        {
            glDeleteVertexArrays(1, &m_modelVertexArray);
            glDeleteBuffers(1, &m_modelVertexBuffer);
            glDeleteBuffers(1, &m_modelIndexBuffer);
        }

        void OpenGLLScene::buildInstanceData()
        {
            glCreateBuffers(1, &m_instanceBuffer);
            glNamedBufferData(
                m_instanceBuffer,
                m_backend->getResourceBudget(BackendResource::SceneInstances),
                nullptr,
                GL_DYNAMIC_DRAW
            );

            glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, m_instanceBuffer);
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
        }
    }
}