#pragma once

#include "Chicane/Renderer.hpp"
#include "Chicane/Renderer/Backend.hpp"
#include "Chicane/Renderer/Draw/Texture/Data.hpp"

namespace Chicane
{
    namespace Renderer
    {
        class CHICANE_RENDERER OpenGLBackend : public Backend<Frame>
        {
        public:
            OpenGLBackend(const Window* inWindow);
            virtual ~OpenGLBackend();

        protected:
            void onInit() override;
            void onLoad(const DrawTexture::List& inResources) override;
            void onSetup() override;
            void onCleanup() override;

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