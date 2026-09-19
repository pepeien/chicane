#pragma once

#include "Chicane/Core/Math/Vertex.hpp"

#include "Chicane/Renderer/Backend.hpp"
#include "Chicane/Renderer/Layer.hpp"
#include "Chicane/Renderer/RHI/CommandList.hpp"
#include "Chicane/Renderer/RHI/Device.hpp"
#include "Chicane/Renderer/RHI/Frame.hpp"
#include "Chicane/Renderer/RHI/FullscreenPass.hpp"

namespace Chicane
{
    namespace Renderer
    {
        inline std::vector<RHI::VertexAttribute> rhiPoly2DAttributes()
        {
            return {
                {0, RHI::VertexFormat::Float3, static_cast<std::uint32_t>(offsetof(Vertex, position))},
                {2, RHI::VertexFormat::Float2, static_cast<std::uint32_t>(offsetof(Vertex, uv))      },
                {3, RHI::VertexFormat::Float3, static_cast<std::uint32_t>(offsetof(Vertex, normal))  }
            };
        }

        inline std::vector<RHI::VertexAttribute> rhiPoly3DPositionAttributes()
        {
            return {
                {0, RHI::VertexFormat::Float3, static_cast<std::uint32_t>(offsetof(Vertex, position))}
            };
        }

        inline std::vector<RHI::VertexAttribute> rhiPoly3DColorAttributes()
        {
            return {
                {0, RHI::VertexFormat::Float3, static_cast<std::uint32_t>(offsetof(Vertex, position))},
                {1, RHI::VertexFormat::Float4, static_cast<std::uint32_t>(offsetof(Vertex, color))   }
            };
        }

        inline std::vector<RHI::VertexAttribute> rhiPoly3DAttributes(bool inTangent = true)
        {
            std::vector<RHI::VertexAttribute> attributes = {
                {0, RHI::VertexFormat::Float3, static_cast<std::uint32_t>(offsetof(Vertex, position))},
                {1, RHI::VertexFormat::Float4, static_cast<std::uint32_t>(offsetof(Vertex, color))   },
                {2, RHI::VertexFormat::Float2, static_cast<std::uint32_t>(offsetof(Vertex, uv))      },
                {3, RHI::VertexFormat::Float3, static_cast<std::uint32_t>(offsetof(Vertex, normal))  }
            };
            if (inTangent)
            {
                attributes.push_back(
                    {4, RHI::VertexFormat::Float4, static_cast<std::uint32_t>(offsetof(Vertex, tangent))}
                );
            }

            return attributes;
        }

        inline void rhiWriteGroup(
            RHI::Device*                          inDevice,
            RHI::BindGroup&                       outGroup,
            RHI::BindGroupLayout                  inLayout,
            const std::vector<RHI::BindResource>& inResources
        )
        {
            if (outGroup.handle)
            {
                inDevice->updateBindGroup(outGroup, inResources);
                return;
            }

            outGroup = inDevice->createBindGroup(inLayout, inResources);
        }

        inline void rhiReplaceGroup(
            RHI::Device*                          inDevice,
            std::vector<RHI::BindGroup>&          outGroups,
            std::uint32_t                         inIndex,
            RHI::BindGroupLayout                  inLayout,
            const std::vector<RHI::BindResource>& inResources
        )
        {
            if (outGroups.size() <= inIndex)
            {
                outGroups.resize(inIndex + 1);
            }
            rhiWriteGroup(inDevice, outGroups[inIndex], inLayout, inResources);
        }

        inline void rhiApplyView(RHI::CommandList* inCommands, Backend* inBackend, Layer* inLayer)
        {
            inCommands->setViewport(inBackend->getRHIViewport(inLayer));
            inCommands->setScissor(inBackend->getRHIScissor(inLayer));
            inCommands->setLineWidth(1.0f);
        }

        inline RHI::PassCreateInfo rhiScenePass(
            const RHI::Frame& inFrame, bool inDepth, RHI::LoadOp inColorLoad, RHI::LoadOp inDepthLoad
        )
        {
            RHI::PassCreateInfo pass;
            pass.bHasColor   = true;
            pass.color.image = inFrame.sceneColor;
            pass.color.load  = inColorLoad;
            pass.color.store = RHI::StoreOp::Store;
            pass.width       = inFrame.width;
            pass.height      = inFrame.height;
            if (inDepth)
            {
                pass.bHasDepth   = true;
                pass.depth.image = inFrame.sceneDepth;
                pass.depth.load  = inDepthLoad;
                pass.depth.store = RHI::StoreOp::Store;
            }

            return pass;
        }
    }
}
