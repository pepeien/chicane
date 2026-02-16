#pragma once

#include "Chicane/Renderer.hpp"
#include "Chicane/Renderer/Backend.hpp"
#include "Chicane/Renderer/Draw/Texture/Data.hpp"

namespace Chicane
{
    namespace Renderer
    {
        class CHICANE_RENDERER OpenGLBackend : public Backend
        {
        public:
            OpenGLBackend();

        protected:
            // Lifecycle
            void onInit() override;
            void onShutdown() override;

            // Event
            void onLoad(const DrawTextureResource& inResources) override;

            // Render
            void onBeginRender() override;
            void onRender(const Frame& inFrame);
            void onEndRender() override;

        private:
            // OpenGL
            void buildContext();
            void destroyContext();
            void buildGlew();
            void enableFeatures();

            void buildTextureData();
            void destroyTextureData();

            // Layer
            void buildLayers();

        private:
            std::uint32_t m_texturesBuffer;
        };
    }
}