#pragma once

#include <cstdint>
#include <vector>

#include "Chicane/Core/String.hpp"

#include "Chicane/Renderer.hpp"
#include "Chicane/Renderer/Culling/FrontFace.hpp"
#include "Chicane/Renderer/Culling/Mode.hpp"
#include "Chicane/Renderer/Depth/Compare.hpp"
#include "Chicane/Renderer/RHI/BindGroup/Layout.hpp"
#include "Chicane/Renderer/RHI/BlendMode.hpp"
#include "Chicane/Renderer/RHI/FillMode.hpp"
#include "Chicane/Renderer/RHI/Image/Format.hpp"
#include "Chicane/Renderer/RHI/Primitive/Topology.hpp"
#include "Chicane/Renderer/RHI/StencilMode.hpp"
#include "Chicane/Renderer/RHI/Vertex/Attribute.hpp"

namespace Chicane
{
    namespace Renderer
    {
        namespace RHI
        {
            struct CHICANE_RENDERER PipelineCreateInfo
            {
            public:
                // Shader
                bool                         bHasPushVertex = false;
                String                       vertexPath     = String::empty();

                bool                         bHasPushFragment = false;
                String                       fragmentPath     = String::empty();

                // Primitive
                std::uint32_t                vertexStride     = 0;
                std::vector<VertexAttribute> vertexAttributes = {};
                PrimitiveTopology            topology         = PrimitiveTopology::TriangleList;

                // Fill
                FillMode                     fill = FillMode::Fill;

                // Culling
                CullingMode                  cull      = CullingMode::None;
                CullingFrontFace             frontFace = CullingFrontFace::CounterClockwise;

                // Stencil
                StencilMode                  stencil = StencilMode::None;

                // Blend
                BlendMode                    blend = BlendMode::None;

                std::uint32_t                pushConstantSize = 0;

                // Color
                bool                         bHasColor      = true;
                bool                         bHasColorWrite = true;
                ImageFormat                  colorFormat    = ImageFormat::RGBA16F;

                // Depth
                bool                         bHasDepth      = false;
                bool                         bHasDepthTest  = false;
                bool                         bHasDepthWrite = false;
                ImageFormat                  depthFormat    = ImageFormat::Depth32F;
                DepthCompare                 depthCompare   = DepthCompare::Less;

                // Layouts
                std::vector<BindGroupLayout> layouts = {};
            };
        }
    }
}
