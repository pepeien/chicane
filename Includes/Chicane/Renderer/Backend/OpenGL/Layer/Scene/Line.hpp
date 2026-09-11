#pragma once

#include <cstddef>
#include <cstdint>

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

            void buildOutlineShader();
            void destroyOutlineShader();

            void buildImmediateVertexArray();
            void destroyImmediateVertexArray();
            void uploadImmediateGeometry(const Frame& inFrame);

            void drawOutlineMeshes(const Frame& inFrame, float inOffsetX, float inOffsetY) const;

            bool shouldDrawMeshWireframe(const Frame& inFrame) const;
            bool shouldDrawLineList(const Frame& inFrame) const;
            bool shouldDrawOutline(const Frame& inFrame) const;

            void drawLineList(const DrawPoly& inDraw) const;

        private:
            std::uint32_t m_meshShaderProgram;
            std::uint32_t m_outlineShaderProgram;

            std::uint32_t m_immediateVertexArray;
            std::uint32_t m_immediateVertexBuffer;
            std::uint32_t m_immediateIndexBuffer;
            std::size_t   m_immediateVertexCapacity;
            std::size_t   m_immediateIndexCapacity;
        };
    }
}
