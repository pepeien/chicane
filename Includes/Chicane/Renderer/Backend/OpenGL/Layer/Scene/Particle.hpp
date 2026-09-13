#pragma once

#include "Chicane/Renderer.hpp"
#include "Chicane/Renderer/Layer.hpp"

namespace Chicane
{
    namespace Renderer
    {
        class CHICANE_RENDERER OpenGLLSceneParticle : public Layer
        {
        public:
            OpenGLLSceneParticle();

        public:
            void onInit() override;
            void onDestruction() override;

            bool onBeginRender(const Frame& inFrame) override;
            void onRender(const Frame& inFrame, void* inData) override;
            void onEndRender() override;

        private:
            void buildShader();
            void destroyShader();

            void buildBuffers();
            void destroyBuffers();

        private:
            std::uint32_t m_shaderProgram;
            std::uint32_t m_vertexArray;
            std::uint32_t m_particleBuffer;
        };
    }
}
