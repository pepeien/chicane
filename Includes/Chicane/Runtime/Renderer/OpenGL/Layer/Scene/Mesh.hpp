#pragma once

#include <GL/glew.h>

#include "Chicane/Box/Asset/Model/Manager.hpp"
#include "Chicane/Box/Asset/Mesh.hpp"
#include "Chicane/Box/Asset/Texture/Manager.hpp"
#include "Chicane/Runtime.hpp"
#include "Chicane/Runtime/Renderer/Layer.hpp"

namespace Chicane
{
    namespace OpenGL
    {
        class CHICANE_RUNTIME LSceneMesh : public RendererLayer
        {
        public:
            LSceneMesh();
            ~LSceneMesh();

        public:
            bool onInit() override;
            void onRender(void* outData) override;

        private:
            // Event
            void loadEvents();

            // Shaders
            void buildShaders();
            void destroyShaders();

            // Texture
            void buildTextureData();
            void destroyTextureData();

            // Model
            void buildModelVertexBuffer();
            void buildModelIndexBuffer();
            void buildModelData();
            void destroyModelData();
            void rebuildModelData();

        private:
            // Shader
            GLuint               m_shaderProgram;

            // Model
            GLuint               m_vao;
            GLuint               m_modelVertexBuffer;
            GLuint               m_modelIndexBuffer;
            Box::ModelManager*   m_modelManager;

            // Texture
            GLuint               m_textureBuffer;
            Box::TextureManager* m_textureManager;
        };
    }
}