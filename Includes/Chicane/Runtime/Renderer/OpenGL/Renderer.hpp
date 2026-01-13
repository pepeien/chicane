#pragma once

#include "Chicane/Runtime/Renderer.hpp"

namespace Chicane
{
    namespace OpenGL
    {
        class CHICANE_RUNTIME Renderer : public Chicane::Renderer
        {
        public:
            Renderer();
            virtual ~Renderer();

        protected:
            void onInit() override;
            void onRender() override;
            void onResizing() override;

        private:
            void buildContext();
            void destroyContext();
            void buildGlew();
            void buildDebugMessenger();
            void buildLayers();
        };
    }
}