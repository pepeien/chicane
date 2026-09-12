#pragma once

#include <array>
#include <cstdint>
#include <vector>

#include <glad/gl.h>

#include "Chicane/Renderer.hpp"
#include "Chicane/Renderer/Backend.hpp"
#include "Chicane/Renderer/Backend/OpenGL/Frame.hpp"
#include "Chicane/Renderer/Blending.hpp"
#include "Chicane/Renderer/Culling.hpp"
#include "Chicane/Renderer/Depth.hpp"
#include "Chicane/Renderer/Draw.hpp"
#include "Chicane/Renderer/Draw/Texture.hpp"
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
            using GpuQueries = std::vector<std::array<std::uint32_t, 2>>;

        public:
            OpenGLBackend();
            ~OpenGLBackend();

        public:
            Draw::Id getScreenTextureId() const override;

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

            // Target
            void bindTarget() const;
            std::uint32_t getTargetColor() const;

        private:
            struct TextureSlot
            {
                std::uint32_t classIndex     = ~0u;
                std::uint32_t layer          = 0;
                std::uint32_t sourceWidth    = 0;
                std::uint32_t sourceHeight   = 0;
                std::uint32_t residentMinMip = ~0u;
            };

            struct SizeClass
            {
                std::uint32_t              size      = 1;
                std::uint32_t              texture   = 0;
                std::uint32_t              allocated = 0;
                std::uint32_t              used      = 0;
                std::vector<std::uint32_t> freeLayers;
            };

            struct TextureTableEntry
            {
                std::uint32_t classIndex = 0;
                std::uint32_t layer      = 0;
            };

            // OpenGL
            void buildContext();
            void destroyContext();
            void buildGlad();
            void enableFeatures();
            void updateResourcesBudget();

            void buildTextureData();
            void destroyTextureData();
            void bindTextureTable() const;
            void uploadTexture(const DrawTexture& inTexture);
            void releaseTextureSlot(TextureSlot& inSlot);
            void writeTextureSlot(Draw::Id inId, std::uint32_t inClass, std::uint32_t inLayer);
            std::uint32_t classFromResident(std::uint32_t inWidth, std::uint32_t inHeight) const;
            std::uint32_t allocateLayer(std::uint32_t inClass);
            void growClass(std::uint32_t inClass, std::uint32_t inLayers);
            void createClassArray(SizeClass& inClass, std::uint32_t inLayers);
            void fillWhiteLayer(const SizeClass& inClass, std::uint32_t inLayer) const;

            void buildTarget();
            void destroyTarget();
            void presentTarget(bool inShouldPresentToWindow) const;
            void captureScreenTarget();

            // GPU timing
            void buildGpuQueries();
            void destroyGpuQueries();
            void beginGpuQuery();
            void endGpuQuery();
            void resolveGpuQuery(std::uint32_t inSlot);

            // Frame
            void buildFrames();
            void destroyFrames();

            // Layer
            void buildLayers();

            // Function
            std::uint16_t toGLDepthCompare(DepthCompare inValue) const;
            std::uint16_t toGLFrontFace(CullingFrontFace inValue) const;
            std::uint16_t toGLCullingMode(CullingMode inValue) const;
            std::uint16_t toGLFactor(BlendingFactor inValue) const;
            std::uint32_t toGLDrawTopology(DrawPolyTopology inValue) const;

        public:
            // Frame
            std::vector<OpenGLFrame> frames;

        private:
            std::uint32_t             m_currentFrameIndex;

            std::vector<TextureSlot>                  m_textures;
            std::array<SizeClass, TEXTURE_CLASS_COUNT> m_classes;
            std::uint32_t                             m_textureTable;
            std::uint32_t                             m_maxArrayLayers;
            std::uint32_t             m_targetFramebuffer;
            std::uint32_t     m_targetColor;
            std::uint32_t     m_targetDepth;
            std::uint32_t     m_targetWidth;
            std::uint32_t     m_targetHeight;
            std::uint32_t     m_screenBlitFramebuffer;
            Draw::Id          m_screenTextureId;

            // GPU timing
            GpuQueries        m_gpuQueries;
            std::vector<bool> m_gpuQueryPending;
            std::uint32_t     m_gpuQueryWrite;
        };
    }
}