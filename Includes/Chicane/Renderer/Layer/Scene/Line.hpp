#pragma once

#include <vector>

#include "Chicane/Core/Math/Vec/Vec4.hpp"

#include "Chicane/Renderer.hpp"
#include "Chicane/Renderer/Layer.hpp"
#include "Chicane/Renderer/RHI/BindGroup.hpp"
#include "Chicane/Renderer/RHI/BindGroup/Layout.hpp"
#include "Chicane/Renderer/RHI/Buffer.hpp"
#include "Chicane/Renderer/RHI/CommandList.hpp"
#include "Chicane/Renderer/RHI/Device.hpp"
#include "Chicane/Renderer/RHI/Pipeline.hpp"

namespace Chicane
{
    namespace Renderer
    {
        class CHICANE_RENDERER LSceneLine : public Layer
        {
        public:
            LSceneLine();

        public:
            void onInit() override;
            void onDestruction() override;

            bool onBeginRender(const Frame& inFrame) override;
            void onRender(const Frame& inFrame, void* inData) override;

        protected:
            virtual Vec4 getWireframeColor() const;
            virtual Vec4 getOutlineColor() const;

        private:
            struct ImmediateGeometry
            {
                RHI::Buffer vertex      = {};
                RHI::Buffer index       = {};
                std::size_t vertexBytes = 0;
                std::size_t indexBytes  = 0;
            };

        private:
            bool shouldDrawMeshWireframe(const Frame& inFrame) const;
            bool shouldDrawLineList(const Frame& inFrame) const;
            bool shouldDrawOutline(const Frame& inFrame) const;
            ImmediateGeometry& ensureImmediate(
                RHI::Device* inDevice, const Frame& inFrame, std::uint32_t inFrameIndex
            );
            void drawOutline(RHI::CommandList* inCommands, const Frame& inFrame, float inOffsetX, float inOffsetY);

        private:
            RHI::Pipeline                   m_meshPipeline;
            RHI::Pipeline                   m_linePipeline;
            RHI::Pipeline                   m_lineForegroundPipeline;
            RHI::Pipeline                   m_outlineMaskPipeline;
            RHI::Pipeline                   m_outlinePipeline;
            RHI::BindGroupLayout            m_layout;
            std::vector<RHI::BindGroup>     m_groups;
            std::vector<ImmediateGeometry>  m_immediate;
        };
    }
}
