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
            static constexpr inline const char* ID = "Engine_Scene_Sky";

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