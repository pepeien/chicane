#pragma once

#include "Chicane/Renderer.hpp"
#include "Chicane/Renderer/Layer.hpp"

namespace Chicane
{
    namespace Renderer
    {
        class CHICANE_RENDERER OpenGLLSceneSky : public Layer
        {
        public:
            OpenGLLSceneSky();
            ~OpenGLLSceneSky();

        public:
            bool onInit() override;
            void onLoad(const DrawSky& inResource) override;
            bool onSetup(const Frame& inFrame) override;
            void onRender(const Frame& inFrame, void* inData) override;
            void onCleanup() override;

        private:
            void buildShader();
            void destroyShader();

            void buildTextureData();
            void destroyTextureData();

        private:
            // Shader
            std::uint32_t m_shaderProgram;

            // Buffer
            std::uint32_t m_texturesBuffer;
        };
    }
}