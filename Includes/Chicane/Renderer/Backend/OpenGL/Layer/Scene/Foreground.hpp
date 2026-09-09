#pragma once

#include "Chicane/Renderer.hpp"
#include "Chicane/Renderer/Layer.hpp"

namespace Chicane
{
    namespace Renderer
    {
        class CHICANE_RENDERER OpenGLLSceneForeground : public Layer
        {
        public:
            OpenGLLSceneForeground();

        public:
            bool onBeginRender(const Frame& inFrame) override;
            void onRender(const Frame& inFrame, void* inData) override;
            void onEndRender() override;
        };
    }
}
