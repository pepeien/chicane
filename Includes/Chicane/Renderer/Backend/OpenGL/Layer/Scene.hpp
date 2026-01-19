#pragma once

#include "Chicane/Renderer.hpp"
#include "Chicane/Renderer/Layer.hpp"

namespace Chicane
{
    namespace Renderer
    {
        class CHICANE_RENDERER OpenGLLScene : public Layer
        {
        public:
            OpenGLLScene();

        public:
            bool onInit() override;
            bool onSetup(const Frame& inFrame) override;

        private:
            // Model
            void buildModelVertexArray();
            void buildModelVertexBuffer();
            void buildModelIndexBuffer();

            // Layer
            void buildLayers();

        private:
            // Model
            std::uint32_t m_modelVertexArray;
            std::uint32_t m_modelVertexBuffer;
            std::uint32_t m_modelIndexBuffer;
        };
    }
}