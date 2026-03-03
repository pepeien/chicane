#pragma once

#include "Chicane/Renderer.hpp"
#include "Chicane/Renderer/Backend.hpp"
#include "Chicane/Renderer/Blending.hpp"
#include "Chicane/Renderer/Culling.hpp"
#include "Chicane/Renderer/Depth.hpp"
#include "Chicane/Renderer/Draw/Texture/Data.hpp"
#include "Chicane/Renderer/Instance.hpp"
#include "Chicane/Renderer/Shader.hpp"

namespace Chicane
{
    namespace Renderer
    {
        class CHICANE_RENDERER OpenGLBackend : public Backend
        {
        public:
            OpenGLBackend();
            ~OpenGLBackend();

        protected:
            // Lifecycle
            void onInit() override;
            void onShutdown() override;

            // Event
            void onLoad(const DrawTextureResource& inResources) override;

            // Render
            void onBeginRender() override;
            void onRender(const Frame& inFrame) override;
            void onEndRender() override;

        public:
            // Layer
            Viewport getGLViewport(Layer* inLayer) const;
            void useViewport(Layer* inLayer) const;

            // Program
            void useProgram(std::uint32_t inId) const;
            void destroyProgram(std::uint32_t inId) const;

            // Shader
            std::uint32_t initShader(const Shader::List& inShaders) const;

            // Vertex Array
            std::uint32_t initVertexArray(std::uint32_t inCount) const;
            void bindVertexArray(std::uint32_t inId) const;
            void destroyVertexArray(std::uint32_t& inId) const;

            // Function
            void enableDepth(const Depth& inSettings) const;
            void disableDepth() const;

            void enableCulling(const Culling& inSettings) const;
            void disableCulling() const;

            void enableBlending(const Blending& inSettings) const;
            void disableBlending() const;

            // Draw
            void drawPolyArrays(const DrawPoly& inSettings, std::uint32_t inVertexArrayId) const;

        private:
            // OpenGL
            void buildContext();
            void destroyContext();
            void buildGlad();
            void enableFeatures();
            void updateResourcesBudget();

            void buildTextureData();
            void destroyTextureData();

            // Layer
            void buildLayers();

            // Function
            std::uint16_t toGLDepthCompare(DepthCompare inValue) const;
            std::uint16_t toGLFrontFace(CullingFrontFace inValue) const;
            std::uint16_t toGLCullingMode(CullingMode inValue) const;
            std::uint16_t toGLFactor(BlendingFactor inValue) const;
            std::uint32_t toGLDrawTopology(DrawPolyTopology inValue) const;

        private:
            std::uint32_t m_texturesBuffer;
        };
    }
}