#include "Chicane/Renderer/Backend/OpenGL/Layer/Scene.hpp"

#include <GL/glew.h>

#include "Chicane/Renderer/Backend/OpenGL/Layer/Scene/Mesh.hpp"

namespace Chicane
{
    namespace Renderer
    {
        OpenGLLScene::OpenGLLScene()
            : Layer("Engine_Scene")
        {
            init();
        }

        OpenGLLScene::~OpenGLLScene()
        {
            destroyCameraData();
            destroyLightData();
            destroyModelData();
            destroyMeshData();
        }

        bool OpenGLLScene::onInit()
        {
            buildCameraData();
            buildLightData();
            buildModelVertexArray();
            buildModelVertexBuffer();
            buildModelIndexBuffer();
            buildMeshData();
            buildLayers();

            return true;
        }

        bool OpenGLLScene::onSetup(const Frame& inFrame)
        {
            glBindBuffer(GL_UNIFORM_BUFFER, m_cameraBuffer);
            glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(View), &inFrame.getCamera());

            glBindBuffer(GL_UNIFORM_BUFFER, m_lightBuffer);
            glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(View), inFrame.getLights().data());

            const Vertex::List& vertices = inFrame.getVertices3D();
            glBindBuffer(GL_ARRAY_BUFFER, m_modelVertexBuffer);
            glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(Vertex) * vertices.size(), vertices.data());

            const Vertex::Indices& indices = inFrame.getIndices3D();
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_modelIndexBuffer);
            glBufferSubData(
                GL_ELEMENT_ARRAY_BUFFER,
                0,
                sizeof(Vertex::Index) * indices.size(),
                indices.data()
            );

            glBindVertexArray(m_modelVertexArray);

            glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_meshBuffer);
            glBufferSubData(
                GL_SHADER_STORAGE_BUFFER,
                0,
                sizeof(Draw3DInstance) * inFrame.getInstances3D().size(),
                inFrame.getInstances3D().data()
            );

            return true;
        }

        void OpenGLLScene::buildCameraData()
        {
            glGenBuffers(1, &m_cameraBuffer);
            glBindBuffer(GL_UNIFORM_BUFFER, m_cameraBuffer);
            glBufferData(GL_UNIFORM_BUFFER, sizeof(View), NULL, GL_DYNAMIC_DRAW);
            glBindBufferBase(GL_UNIFORM_BUFFER, 0, m_cameraBuffer);
        }

        void OpenGLLScene::destroyCameraData()
        {
            glDeleteBuffers(1, &m_cameraBuffer);
        }

        void OpenGLLScene::buildLightData()
        {
            glGenBuffers(1, &m_lightBuffer);
            glBindBuffer(GL_UNIFORM_BUFFER, m_lightBuffer);
            glBufferData(GL_UNIFORM_BUFFER, sizeof(View), NULL, GL_DYNAMIC_DRAW);
            glBindBufferBase(GL_UNIFORM_BUFFER, 1, m_lightBuffer);
        }

        void OpenGLLScene::destroyLightData()
        {
            glDeleteBuffers(1, &m_lightBuffer);
        }

        void OpenGLLScene::buildModelVertexArray()
        {
            glGenVertexArrays(1, &m_modelVertexArray);
            glBindVertexArray(m_modelVertexArray);
        }

        void OpenGLLScene::buildModelVertexBuffer()
        {
            glGenBuffers(1, &m_modelVertexBuffer);
            glBindBuffer(GL_ARRAY_BUFFER, m_modelVertexBuffer);
            glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * 2000000, NULL, GL_DYNAMIC_DRAW);

            glVertexAttribPointer(
                0,
                3,
                GL_FLOAT,
                GL_FALSE,
                sizeof(Vertex),
                (void*)offsetof(Vertex, position)
            );
            glEnableVertexAttribArray(0);

            glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, color));
            glEnableVertexAttribArray(1);

            glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, uv));
            glEnableVertexAttribArray(2);

            glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
            glEnableVertexAttribArray(3);
        }

        void OpenGLLScene::buildModelIndexBuffer()
        {
            glGenBuffers(1, &m_modelIndexBuffer);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_modelIndexBuffer);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(std::uint32_t) * 2000000, NULL, GL_DYNAMIC_DRAW);
        }

        void OpenGLLScene::destroyModelData()
        {
            glDeleteVertexArrays(1, &m_modelVertexArray);
            glDeleteBuffers(1, &m_modelVertexBuffer);
            glDeleteBuffers(1, &m_modelIndexBuffer);
        }

        void OpenGLLScene::buildMeshData()
        {
            glGenBuffers(1, &m_meshBuffer);
            glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_meshBuffer);
            glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(Draw3DInstance) * 1000, NULL, GL_DYNAMIC_DRAW);
            glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, m_meshBuffer);
        }

        void OpenGLLScene::destroyMeshData()
        {
            glDeleteBuffers(1, &m_meshBuffer);
        }

        void OpenGLLScene::buildLayers()
        {
            addLayer<OpenGLLSceneMesh>();
        }
    }
}