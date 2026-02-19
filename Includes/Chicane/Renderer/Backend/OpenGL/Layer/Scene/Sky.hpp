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

        public:
            void onInit() override;
            void onDestruction() override;

            void onLoad(const DrawSky& inResource) override;

            bool onBeginRender(const Frame& inFrame) override;
            void onRender(const Frame& inFrame, void* inData) override;
            void onEndRender() override;

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