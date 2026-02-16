#pragma once

#include "Chicane/Renderer.hpp"
#include "Chicane/Renderer/Layer.hpp"

namespace Chicane
{
    namespace Renderer
    {
        class CHICANE_RENDERER OpenGLLSceneMesh : public Layer
        {
        public:
            static constexpr inline const char* ID = "Engine_Scene_Mesh";

        public:
            OpenGLLSceneMesh();

        public:
            void onInit() override;
            void onDestruction() override;

            bool onBeginRender(const Frame& inFrame) override;
            void onRender(const Frame& inFrame, void* inData) override;
            void onEndRender() override;

        private:
            void buildShader();
            void destroyShader();

        private:
            // Shader
            std::uint32_t m_shaderProgram;
        };
    }
}