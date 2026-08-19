#pragma once

#include "Chicane/Renderer.hpp"
#include "Chicane/Renderer/Layer.hpp"

namespace Chicane
{
    namespace Renderer
    {
        class CHICANE_RENDERER OpenGLLUI : public Layer
        {
        public:
            OpenGLLUI();

        public:
            void onInit() override;
            void onDestruction() override;

            void onLoad(DrawPolyType inType, const DrawPolyResource& inResource) override;

            bool onBeginRender(const Frame& inFrame) override;
            void onRender(const Frame& inFrame, void* inData) override;
            void onEndRender() override;

        private:
            // Shader
            void buildShader();
            void destroyShader();

            // Component
            void buildPrimitiveVertexArray();
            void buildPrimitiveVertexBuffer();
            void buildPrimitiveIndexBuffer();
            void destroyPrimitiveData();

            // Size
            void buildInstanceData();
            void destroyInstanceData();

            // Glyph
            void buildGlyphBuffer();
            void destroyGlyphData();

            // Backdrop
            void ensureBackdrop(std::uint32_t inWidth, std::uint32_t inHeight);
            void copyBackdrop(const Viewport& inViewport);
            void destroyBackdrop();

        private:
            // Shader
            std::uint32_t m_shaderProgram;

            // Primitive
            std::uint32_t m_primitiveVertexArray;
            std::uint32_t m_primitiveVertexBuffer;
            std::uint32_t m_primitiveIndexBuffer;

            // Instance
            std::uint32_t m_instanceBuffer;

            // Glyph
            std::uint32_t m_glyphBuffer;

            // Backdrop
            std::uint32_t m_backdropTexture;
            std::uint32_t m_backdropFramebuffer;
            std::uint32_t m_backdropWidth;
            std::uint32_t m_backdropHeight;
        };
    }
}