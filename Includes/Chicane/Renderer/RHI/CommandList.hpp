#pragma once

#include <cstdint>

#include "Chicane/Renderer.hpp"
#include "Chicane/Renderer/RHI/BindGroup.hpp"
#include "Chicane/Renderer/RHI/Buffer.hpp"
#include "Chicane/Renderer/RHI/Image.hpp"
#include "Chicane/Renderer/RHI/Pass/CreateInfo.hpp"
#include "Chicane/Renderer/RHI/Pipeline.hpp"
#include "Chicane/Renderer/RHI/Scissor.hpp"
#include "Chicane/Renderer/RHI/Viewport.hpp"

namespace Chicane
{
    namespace Renderer
    {
        namespace RHI
        {
            class CHICANE_RENDERER CommandList
            {
            public:
                virtual ~CommandList() = default;

                virtual void beginPass(const PassCreateInfo& inCreateInfo)           = 0;
                virtual void endPass()                                               = 0;
                virtual void bindPipeline(Pipeline inPipeline)                       = 0;
                virtual void bindGroup(std::uint32_t inSet, BindGroup inGroup)       = 0;
                virtual void bindVertexBuffer(Buffer inBuffer)                       = 0;
                virtual void bindIndexBuffer(Buffer inBuffer)                        = 0;
                virtual void pushConstants(const void* inData, std::uint32_t inSize) = 0;
                virtual void draw(
                    std::uint32_t inVertexCount,
                    std::uint32_t inInstanceCount,
                    std::uint32_t inFirstVertex,
                    std::uint32_t inFirstInstance
                ) = 0;
                virtual void drawIndexed(
                    std::uint32_t inIndexCount,
                    std::uint32_t inInstanceCount,
                    std::uint32_t inFirstIndex,
                    std::int32_t  inVertexOffset,
                    std::uint32_t inFirstInstance
                )                                                    = 0;
                virtual void setViewport(const Viewport& inViewport) = 0;
                virtual void setScissor(const Scissor& inScissor)    = 0;
                virtual void setLineWidth(float inWidth)             = 0;
                virtual void blitColor(
                    Image         inSource,
                    Image         inDestination,
                    std::int32_t  inX,
                    std::int32_t  inY,
                    std::uint32_t inWidth,
                    std::uint32_t inHeight
                )                                             = 0;
                virtual void prepareShaderRead(Image inImage) = 0;
                virtual void preparePresent(Image inImage)    = 0;
            };
        }
    }
}
