#pragma once

#include <vector>

#include "Chicane/Renderer.hpp"
#include "Chicane/Renderer/Layer.hpp"
#include "Chicane/Renderer/Layer/UI/Backdrop.hpp"
#include "Chicane/Renderer/RHI/BindGroup.hpp"
#include "Chicane/Renderer/RHI/BindGroup/Layout.hpp"
#include "Chicane/Renderer/RHI/Buffer.hpp"
#include "Chicane/Renderer/RHI/Device.hpp"
#include "Chicane/Renderer/RHI/Image.hpp"
#include "Chicane/Renderer/RHI/Pipeline.hpp"
#include "Chicane/Renderer/RHI/Sampler.hpp"

namespace Chicane
{
    namespace Renderer
    {
        class CHICANE_RENDERER LUI : public Layer
        {
        public:
            LUI();

        public:
            void onInit() override;
            void onDestruction() override;

            void onLoad(DrawPolyType inType, const DrawPolyResource& inResource) override;

            bool onBeginRender(const Frame& inFrame) override;
            void onRender(const Frame& inFrame, void* inData) override;

        private:
            void ensureBackdrop(
                RHI::Device* inDevice, std::uint32_t inIndex, std::uint32_t inWidth, std::uint32_t inHeight
            );

        private:
            RHI::Pipeline               m_pipeline;
            RHI::BindGroupLayout        m_layout;
            RHI::BindGroupLayout        m_textureLayout;
            std::vector<RHI::BindGroup> m_groups;
            RHI::Buffer                 m_vertexBuffer;
            RHI::Buffer                 m_indexBuffer;
            RHI::Buffer                 m_glyphBuffer;
            RHI::Sampler                m_backdropSampler;
            std::vector<UIBackdrop>     m_backdrops;
        };
    }
}
