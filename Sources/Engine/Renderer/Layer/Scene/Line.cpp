#include "Chicane/Renderer/Layer/Scene/Line.hpp"

#include <array>
#include <cstring>

#include "Chicane/Renderer/Backend.hpp"
#include "Chicane/Renderer/Draw/Poly.hpp"
#include "Chicane/Renderer/Draw/Poly/Topology.hpp"
#include "Chicane/Renderer/Feature.hpp"
#include "Chicane/Renderer/Layer/Scene.hpp"
#include "Chicane/Renderer/Layer/Util.hpp"
#include "Chicane/Renderer/RHI/Outline/Push.hpp"
#include "Chicane/Renderer/Shader/Bindings.hpp"

namespace Chicane
{
    namespace Renderer
    {
        LSceneLine::LSceneLine()
            : Layer(SCENE_LINE_LAYER_ID)
        {}

        void LSceneLine::onInit()
        {
            RHI::Device* device = m_backend->getRHIDevice();

            RHI::BindGroupLayoutCreateInfo layout;
            RHI::Binding                   binding;
            binding.binding     = RHI_BINDING_CAMERA;
            binding.type        = RHI::BindingType::UniformBuffer;
            binding.bIsVertex   = true;
            binding.bIsFragment = false;
            layout.bindings.push_back(binding);
            binding.binding     = RHI_BINDING_MESH_INSTANCES;
            binding.type        = RHI::BindingType::StorageBuffer;
            binding.bIsVertex   = true;
            binding.bIsFragment = false;
            layout.bindings.push_back(binding);
            m_layout = device->createBindGroupLayout(layout);

            auto makeMesh = [&](RHI::FillMode                            inFill,
                                RHI::PrimitiveTopology                   inTopology,
                                bool                                     inDepth,
                                RHI::StencilMode                         inStencil,
                                bool                                     inColorWrite,
                                std::uint32_t                            inPush,
                                const String&                            inVertex,
                                const String&                            inFragment,
                                const std::vector<RHI::VertexAttribute>& inAttributes)
            {
                RHI::PipelineCreateInfo desc;
                desc.vertexPath       = inVertex;
                desc.fragmentPath     = inFragment;
                desc.vertexStride     = sizeof(Vertex);
                desc.vertexAttributes = inAttributes;
                desc.topology         = inTopology;
                desc.fill             = inFill;
                desc.cull             = CullingMode::None;
                desc.bHasDepthTest    = inDepth;
                desc.bHasDepthWrite   = false;
                desc.depthCompare     = DepthCompare::LessOrEqual;
                desc.stencil          = inStencil;
                desc.blend            = RHI::BlendMode::None;
                desc.bHasColorWrite   = inColorWrite;
                desc.bHasColor        = true;
                desc.bHasDepth        = true;
                desc.colorFormat      = device->sceneColorFormat();
                desc.depthFormat      = device->sceneDepthFormat();
                desc.pushConstantSize = inPush;
                desc.bHasPushVertex   = inPush > 0;
                desc.bHasPushFragment = inPush > 0;
                desc.layouts          = {m_layout};
                return device->createPipeline(desc);
            };

            m_meshPipeline = makeMesh(
                RHI::FillMode::Line,
                RHI::PrimitiveTopology::TriangleList,
                true,
                RHI::StencilMode::None,
                true,
                0,
                "Assets/Engine/Shaders/Scene/Line",
                "Assets/Engine/Shaders/Scene/Line",
                rhiPoly3DColorAttributes()
            );
            m_linePipeline = makeMesh(
                RHI::FillMode::Fill,
                RHI::PrimitiveTopology::LineList,
                true,
                RHI::StencilMode::None,
                true,
                0,
                "Assets/Engine/Shaders/Scene/Line",
                "Assets/Engine/Shaders/Scene/Line",
                rhiPoly3DColorAttributes()
            );
            m_lineForegroundPipeline = makeMesh(
                RHI::FillMode::Fill,
                RHI::PrimitiveTopology::LineList,
                false,
                RHI::StencilMode::None,
                true,
                0,
                "Assets/Engine/Shaders/Scene/Line",
                "Assets/Engine/Shaders/Scene/Line",
                rhiPoly3DColorAttributes()
            );
            m_outlineMaskPipeline = makeMesh(
                RHI::FillMode::Fill,
                RHI::PrimitiveTopology::TriangleList,
                true,
                RHI::StencilMode::WriteReplace,
                false,
                sizeof(RHI::OutlinePush),
                "Assets/Engine/Shaders/Scene/Outline",
                "Assets/Engine/Shaders/Scene/Outline",
                rhiPoly3DPositionAttributes()
            );
            m_outlinePipeline = makeMesh(
                RHI::FillMode::Fill,
                RHI::PrimitiveTopology::TriangleList,
                true,
                RHI::StencilMode::TestNotEqual,
                true,
                sizeof(RHI::OutlinePush),
                "Assets/Engine/Shaders/Scene/Outline",
                "Assets/Engine/Shaders/Scene/Outline",
                rhiPoly3DPositionAttributes()
            );
        }

        void LSceneLine::onDestruction()
        {
            RHI::Device* device = m_backend->getRHIDevice();
            if (!device)
            {
                return;
            }

            if (m_immediateVertex.handle)
            {
                device->destroyBuffer(m_immediateVertex);
            }
            if (m_immediateIndex.handle)
            {
                device->destroyBuffer(m_immediateIndex);
            }
            for (RHI::BindGroup group : m_groups)
            {
                device->destroyBindGroup(group);
            }
            device->destroyPipeline(m_meshPipeline);
            device->destroyPipeline(m_linePipeline);
            device->destroyPipeline(m_lineForegroundPipeline);
            device->destroyPipeline(m_outlineMaskPipeline);
            device->destroyPipeline(m_outlinePipeline);
            device->destroyBindGroupLayout(m_layout);
        }

        bool LSceneLine::shouldDrawMeshWireframe(const Frame& inFrame) const
        {
            if (inFrame.hasDraws(DrawPolyType::e3D, DrawPolyMode::Fill) &&
                inFrame.hasFeature(RendererFeature::Wireframe))
            {
                return true;
            }

            for (const DrawPoly& draw : inFrame.getDraws(DrawPolyType::e3D, DrawPolyMode::Line))
            {
                if (!draw.isLineList())
                {
                    return true;
                }
            }

            return false;
        }

        bool LSceneLine::shouldDrawLineList(const Frame& inFrame) const
        {
            for (const DrawPoly& draw : inFrame.getDraws(DrawPolyType::e3D, DrawPolyMode::Line))
            {
                if (draw.isLineList())
                {
                    return true;
                }
            }

            return false;
        }

        bool LSceneLine::shouldDrawOutline(const Frame& inFrame) const
        {
            return inFrame.hasOutlineDraws();
        }

        bool LSceneLine::onBeginRender(const Frame& inFrame)
        {
            return shouldDrawMeshWireframe(inFrame) || shouldDrawLineList(inFrame) || shouldDrawOutline(inFrame);
        }

        void LSceneLine::ensureImmediate(RHI::Device* inDevice, const Frame& inFrame)
        {
            const std::size_t vertexBytes = sizeof(Vertex) * inFrame.getImmediateVertices().size();
            const std::size_t indexBytes  = sizeof(Vertex::Index) * inFrame.getImmediateIndices().size();

            if (vertexBytes > m_immediateVertexBytes)
            {
                if (m_immediateVertex.handle)
                {
                    inDevice->destroyBuffer(m_immediateVertex);
                }
                RHI::BufferCreateInfo desc;
                desc.size              = vertexBytes;
                desc.usage             = RHI::BufferUsage::Vertex;
                desc.bHasHostAccess    = true;
                m_immediateVertex      = inDevice->createBuffer(desc);
                m_immediateVertexBytes = vertexBytes;
            }
            if (indexBytes > m_immediateIndexBytes)
            {
                if (m_immediateIndex.handle)
                {
                    inDevice->destroyBuffer(m_immediateIndex);
                }
                RHI::BufferCreateInfo desc;
                desc.size             = indexBytes;
                desc.usage            = RHI::BufferUsage::Index;
                desc.bHasHostAccess   = true;
                m_immediateIndex      = inDevice->createBuffer(desc);
                m_immediateIndexBytes = indexBytes;
            }

            if (vertexBytes > 0)
            {
                inDevice->updateBuffer(m_immediateVertex, inFrame.getImmediateVertices().data(), vertexBytes);
            }
            if (indexBytes > 0)
            {
                inDevice->updateBuffer(m_immediateIndex, inFrame.getImmediateIndices().data(), indexBytes);
            }
        }

        void LSceneLine::drawOutline(
            RHI::CommandList* inCommands, const Frame& inFrame, float inOffsetX, float inOffsetY
        )
        {
            RHI::OutlinePush push;
            push.pixelOffset[0] = inOffsetX;
            push.pixelOffset[1] = inOffsetY;
            push.color[0]       = static_cast<float>(OUTLINE_COLOR.r) / 255.0f;
            push.color[1]       = static_cast<float>(OUTLINE_COLOR.g) / 255.0f;
            push.color[2]       = static_cast<float>(OUTLINE_COLOR.b) / 255.0f;
            push.color[3]       = static_cast<float>(OUTLINE_COLOR.a) / 255.0f;
            inCommands->pushConstants(&push, sizeof(push));

            for (const DrawPoly& draw : inFrame.getOutlineDraws())
            {
                inCommands->drawIndexed(
                    draw.indexCount,
                    draw.instanceCount,
                    draw.indexStart,
                    draw.vertexStart,
                    draw.instanceStart
                );
            }
        }

        void LSceneLine::onRender(const Frame& inFrame, void* inData)
        {
            RHI::Frame*  rhi    = static_cast<RHI::Frame*>(inData);
            LScene*      parent = m_backend->getLayer<LScene>(SCENE_LAYER_ID);
            RHI::Device* device = m_backend->getRHIDevice();
            if (!parent)
            {
                return;
            }

            rhiReplaceGroup(
                device,
                m_groups,
                rhi->frameIndex,
                m_layout,
                {
                    {RHI_BINDING_CAMERA,         RHI::BindingType::UniformBuffer, rhi->cameraBuffer,     {}, {}, 0},
                    {RHI_BINDING_MESH_INSTANCES, RHI::BindingType::StorageBuffer, rhi->instance3DBuffer, {}, {}, 0}
            }
            );

            rhi->commands->beginPass(rhiScenePass(*rhi, true, RHI::LoadOp::Load, RHI::LoadOp::Load));
            rhiApplyView(rhi->commands, m_backend, this);

            if (shouldDrawMeshWireframe(inFrame))
            {
                rhi->commands->bindPipeline(m_meshPipeline);
                rhi->commands->bindGroup(0, m_groups[rhi->frameIndex]);
                rhi->commands->bindVertexBuffer(parent->modelVertexBuffer);
                rhi->commands->bindIndexBuffer(parent->modelIndexBuffer);

                auto drawBatch = [&](DrawPolyMode inMode)
                {
                    for (const DrawPoly& draw : inFrame.getDraws(DrawPolyType::e3D, inMode))
                    {
                        if (draw.isLineList())
                        {
                            continue;
                        }

                        rhi->commands->drawIndexed(
                            draw.indexCount,
                            draw.instanceCount,
                            draw.indexStart,
                            draw.vertexStart,
                            draw.instanceStart
                        );
                    }
                };

                if (inFrame.hasDraws(DrawPolyType::e3D, DrawPolyMode::Line))
                {
                    drawBatch(DrawPolyMode::Line);
                }
                if (inFrame.hasFeature(RendererFeature::Wireframe))
                {
                    for (const DrawPoly& draw : inFrame.getSceneDraws())
                    {
                        rhi->commands->drawIndexed(
                            draw.indexCount,
                            draw.instanceCount,
                            draw.indexStart,
                            draw.vertexStart,
                            draw.instanceStart
                        );
                    }
                }
            }

            if (shouldDrawOutline(inFrame))
            {
                rhi->commands->bindPipeline(m_outlineMaskPipeline);
                rhi->commands->bindGroup(0, m_groups[rhi->frameIndex]);
                rhi->commands->bindVertexBuffer(parent->modelVertexBuffer);
                rhi->commands->bindIndexBuffer(parent->modelIndexBuffer);
                drawOutline(rhi->commands, inFrame, 0.0f, 0.0f);

                rhi->commands->bindPipeline(m_outlinePipeline);
                rhi->commands->bindGroup(0, m_groups[rhi->frameIndex]);
                rhi->commands->bindVertexBuffer(parent->modelVertexBuffer);
                rhi->commands->bindIndexBuffer(parent->modelIndexBuffer);

                const RHI::Viewport viewport = m_backend->getRHIViewport(this);
                const float         scaleX   = viewport.size.x > 0.0f ? 4.0f / viewport.size.x : 0.0f;
                const float         scaleY   = viewport.size.y > 0.0f ? 4.0f / viewport.size.y : 0.0f;
                static const std::array<std::array<float, 2>, 8> offsets = {
                    {{1.0f, 0.0f},
                     {-1.0f, 0.0f},
                     {0.0f, 1.0f},
                     {0.0f, -1.0f},
                     {0.70710678f, 0.70710678f},
                     {0.70710678f, -0.70710678f},
                     {-0.70710678f, 0.70710678f},
                     {-0.70710678f, -0.70710678f}}
                };
                for (const std::array<float, 2>& offset : offsets)
                {
                    drawOutline(rhi->commands, inFrame, offset[0] * scaleX, offset[1] * scaleY);
                }
            }

            if (shouldDrawLineList(inFrame) && inFrame.hasImmediateVertices())
            {
                ensureImmediate(device, inFrame);
                rhi->commands->bindVertexBuffer(m_immediateVertex);
                for (const DrawPoly& draw : inFrame.getDraws(DrawPolyType::e3D, DrawPolyMode::Line))
                {
                    if (!draw.isLineList())
                    {
                        continue;
                    }

                    rhi->commands->bindPipeline(
                        inFrame.isForegroundDraw(draw) ? m_lineForegroundPipeline : m_linePipeline
                    );
                    rhi->commands->bindGroup(0, m_groups[rhi->frameIndex]);
                    rhi->commands->setLineWidth(1.0f);
                    if (draw.indexCount == 0)
                    {
                        rhi->commands->draw(draw.vertexCount, draw.instanceCount, draw.vertexStart, draw.instanceStart);
                    }
                    else
                    {
                        rhi->commands->bindIndexBuffer(m_immediateIndex);
                        rhi->commands->drawIndexed(
                            draw.indexCount,
                            draw.instanceCount,
                            draw.indexStart,
                            draw.vertexStart,
                            draw.instanceStart
                        );
                    }
                }
            }

            rhi->commands->endPass();
        }
    }
}
