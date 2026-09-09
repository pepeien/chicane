#pragma once

#include "Chicane/Core/Math/Vertex.hpp"

#include "Chicane/Renderer.hpp"
#include "Chicane/Renderer/Layer.hpp"

namespace Chicane
{
    namespace Renderer
    {
        class CHICANE_RENDERER OpenGLLSceneLine : public Layer
        {
        public:
            OpenGLLSceneLine();

        public:
            void onInit() override;
            void onDestruction() override;

            bool onBeginRender(const Frame& inFrame) override;
            void onRender(const Frame& inFrame, void* inData) override;
            void onEndRender() override;

        private:
            void buildMeshShader();
            void destroyMeshShader();

            void buildOverlayShader();
            void destroyOverlayShader();

            void buildOutlineShader();
            void destroyOutlineShader();

            void buildOverlayVertexArray();
            void destroyOverlayVertexArray();

            void uploadOverlayBuffer(const Vertex::List& inVertices);

            void drawOutlineMeshes(const Frame& inFrame, float inOffsetX, float inOffsetY) const;

            bool shouldDrawMeshWireframe(const Frame& inFrame) const;
            bool shouldDrawOverlay() const;
            bool shouldDrawOutline(const Frame& inFrame) const;

        private:
            std::uint32_t m_meshShaderProgram;
            std::uint32_t m_overlayShaderProgram;
            std::uint32_t m_outlineShaderProgram;

            std::uint32_t m_overlayVertexArray;
            std::uint32_t m_overlayVertexBuffer;
            std::uint32_t m_overlayVertexCount;
        };
    }
}
