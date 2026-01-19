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

        bool OpenGLLScene::onInit()
        {
            buildModelVertexArray();
            buildModelVertexBuffer();
            buildModelIndexBuffer();
            buildLayers();

            return true;
        }

        bool OpenGLLScene::onSetup(const Frame& inFrame)
        {
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

            return true;
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

            glVertexAttribPointer(
                1,
                4,
                GL_FLOAT,
                GL_FALSE,
                sizeof(Vertex),
                (void*)offsetof(Vertex, color)
            );
            glEnableVertexAttribArray(1);

            glVertexAttribPointer(
                2,
                2,
                GL_FLOAT,
                GL_FALSE,
                sizeof(Vertex),
                (void*)offsetof(Vertex, uv)
            );
            glEnableVertexAttribArray(2);

            glVertexAttribPointer(
                3,
                3,
                GL_FLOAT,
                GL_FALSE,
                sizeof(Vertex),
                (void*)offsetof(Vertex, normal)
            );
            glEnableVertexAttribArray(3);
        }

        void OpenGLLScene::buildModelIndexBuffer()
        {
            glGenBuffers(1, &m_modelIndexBuffer);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_modelIndexBuffer);
            glBufferData(
                GL_ELEMENT_ARRAY_BUFFER,
                sizeof(std::uint32_t) * 2000000,
                NULL,
                GL_DYNAMIC_DRAW
            );
        }

        void OpenGLLScene::buildLayers()
        {
            addLayer<OpenGLLSceneMesh>();
        }
    }
}