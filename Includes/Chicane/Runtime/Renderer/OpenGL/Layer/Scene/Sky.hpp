#pragma once

#include <GL/glew.h>

#include "Chicane/Box/Asset/Model/Manager.hpp"
#include "Chicane/Box/Asset/Sky.hpp"
#include "Chicane/Box/Asset/Texture/Manager.hpp"
#include "Chicane/Runtime.hpp"
#include "Chicane/Runtime/Renderer/Layer.hpp"

namespace Chicane
{
    namespace OpenGL
    {
        class CHICANE_RUNTIME LSceneSky : public RendererLayer
        {
        public:
            LSceneSky();
            ~LSceneSky();

        public:
            bool onInit() override;
            bool onSetup() override;
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
            void buildModelVertexArray();
            void buildModelVertexBuffer();
            void buildModelIndexBuffer();
            void buildModelData();
            void destroyModelData();
            void rebuildModelData();

        private:
            // Texture
            GLuint               m_textureBuffer;

            // Shader
            GLuint               m_shaderProgram;

            // Model
            GLuint               m_modelVertexArray;
            GLuint               m_modelVertexBuffer;
            GLuint               m_modelIndexBuffer;

            // Box
            const Box::Sky*      m_asset;

            Box::ModelManager*   m_modelManager;
            Box::TextureManager* m_textureManager;
        };
    }
}