#pragma once

#include <vector>

#include "Chicane/Renderer.hpp"
#include "Chicane/Renderer/Layer.hpp"
#include "Chicane/Renderer/RHI/BindGroup.hpp"
#include "Chicane/Renderer/RHI/BindGroup/Layout.hpp"
#include "Chicane/Renderer/RHI/Pipeline.hpp"

namespace Chicane
{
    namespace Renderer
    {
        class CHICANE_RENDERER LSceneParticle : public Layer
        {
        public:
            LSceneParticle();

        public:
            void onInit() override;
            void onDestruction() override;

            bool onBeginRender(const Frame& inFrame) override;
            void onRender(const Frame& inFrame, void* inData) override;

        private:
            RHI::Pipeline               m_pipeline;
            RHI::BindGroupLayout        m_layout;
            std::vector<RHI::BindGroup> m_groups;
        };
    }
}
