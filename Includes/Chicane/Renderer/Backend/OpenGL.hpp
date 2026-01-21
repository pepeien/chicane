#pragma once

#include "Chicane/Renderer.hpp"
#include "Chicane/Renderer/Backend.hpp"

namespace Chicane
{
    namespace Renderer
    {
        class CHICANE_RENDERER OpenGLBackend : public Backend
        {
        public:
            OpenGLBackend(const Instance* inRenderer);
            virtual ~OpenGLBackend();

        protected:
            void onInit() override;
            void onSetup(const Frame& inFrame, const DrawResource::Map& inResources) override;
            void onCleanup() override;

        private:
            // OpenGL
            void buildContext();
            void destroyContext();
            void buildGlew();
            void enableFeatures();

            // Layer
            void buildLayers();
        };
    }
}