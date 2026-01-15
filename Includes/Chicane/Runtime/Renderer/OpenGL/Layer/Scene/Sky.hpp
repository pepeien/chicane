#pragma once

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
            bool onCleanup() override;

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
            // Shader
            std::uint32_t        m_shaderProgram;

            // Texture
            std::uint32_t        m_textureBuffer;
            Box::TextureManager* m_textureManager;
            const Box::Sky*      m_asset;

            // Model
            std::uint32_t        m_modelVertexArray;
            std::uint32_t        m_modelVertexBuffer;
            std::uint32_t        m_modelIndexBuffer;
            Box::ModelManager*   m_modelManager;
        };
    }
}