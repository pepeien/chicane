#pragma once

#include "Chicane/Renderer/RHI/CommandList.hpp"
#include "Chicane/Renderer/Backend/OpenGL/RHI/Device.hpp"

namespace Chicane
{
    namespace Renderer
    {
        class CHICANE_RENDERER OpenGLRHICommandList : public RHI::CommandList
        {
        public:
            explicit OpenGLRHICommandList(OpenGLRHIDevice* inDevice);

            void beginPass(const RHI::PassCreateInfo& inCreateInfo) override;
            void endPass() override;
            void bindPipeline(RHI::Pipeline inPipeline) override;
            void bindGroup(std::uint32_t inSet, RHI::BindGroup inGroup) override;
            void bindVertexBuffer(RHI::Buffer inBuffer) override;
            void bindIndexBuffer(RHI::Buffer inBuffer) override;
            void pushConstants(const void* inData, std::uint32_t inSize) override;
            void draw(
                std::uint32_t inVertexCount,
                std::uint32_t inInstanceCount,
                std::uint32_t inFirstVertex,
                std::uint32_t inFirstInstance
            ) override;
            void drawIndexed(
                std::uint32_t inIndexCount,
                std::uint32_t inInstanceCount,
                std::uint32_t inFirstIndex,
                std::int32_t  inVertexOffset,
                std::uint32_t inFirstInstance
            ) override;
            void setViewport(const RHI::Viewport& inViewport) override;
            void setScissor(const RHI::Scissor& inScissor) override;
            void setLineWidth(float inWidth) override;
            void blitColor(
                RHI::Image    inSource,
                RHI::Image    inDestination,
                std::int32_t  inX,
                std::int32_t  inY,
                std::uint32_t inWidth,
                std::uint32_t inHeight
            ) override;
            void prepareShaderRead(RHI::Image inImage) override;
            void preparePresent(RHI::Image inImage) override;

        private:
            void applyPipelineState(const RHI::PipelineCreateInfo& inCreateInfo);
            void applyGroup(const OpenGLRHIGroupData* inGroup);

        private:
            OpenGLRHIDevice*       m_device;
            OpenGLRHIPipelineData* m_pipeline    = nullptr;
            std::uint32_t          m_indexBuffer = 0;
            RHI::PrimitiveTopology m_topology    = RHI::PrimitiveTopology::TriangleList;
        };
    }
}
