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
        class CHICANE_RENDERER LSceneSky : public Layer
        {
        public:
            LSceneSky();

        public:
            void onInit() override;
            void onDestruction() override;

            void onLoad(const DrawSkyResource& inResource) override;

            bool onBeginRender(const Frame& inFrame) override;
            void onRender(const Frame& inFrame, void* inData) override;

        private:
            RHI::Pipeline               m_pipeline;
            RHI::BindGroupLayout        m_frameLayout;
            RHI::BindGroupLayout        m_textureLayout;
            std::vector<RHI::BindGroup> m_frameGroups;
            RHI::BindGroup              m_textureGroup;
            bool                        m_bHasSky = false;
        };
    }
}
