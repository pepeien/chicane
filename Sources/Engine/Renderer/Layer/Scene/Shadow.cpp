#include "Chicane/Renderer/Layer/Scene/Shadow.hpp"

#include "Chicane/Renderer/Backend.hpp"
#include "Chicane/Renderer/Feature.hpp"
#include "Chicane/Renderer/Layer/Scene.hpp"
#include "Chicane/Renderer/Layer/Util.hpp"
#include "Chicane/Renderer/RHI/Shadow/Push.hpp"
#include "Chicane/Renderer/Shader/Bindings.hpp"

namespace Chicane
{
    namespace Renderer
    {
        LSceneShadow::LSceneShadow()
            : Layer(SCENE_SHADOW_LAYER_ID)
        {}

        void LSceneShadow::onInit()
        {
            RHI::Device* device = m_backend->getRHIDevice();

            RHI::BindGroupLayoutCreateInfo layout;
            RHI::Binding                   binding;
            binding.binding     = RHI_BINDING_LIGHT;
            binding.type        = RHI::BindingType::StorageBuffer;
            binding.bIsVertex   = true;
            binding.bIsFragment = false;
            layout.bindings.push_back(binding);
            binding.binding     = RHI_BINDING_MESH_INSTANCES;
            binding.type        = RHI::BindingType::StorageBuffer;
            binding.bIsVertex   = true;
            binding.bIsFragment = false;
            layout.bindings.push_back(binding);
            m_layout = device->createBindGroupLayout(layout);

            RHI::PipelineCreateInfo desc;
            desc.vertexPath       = "Assets/Engine/Shaders/Scene/Shadow";
            desc.vertexStride     = sizeof(Vertex);
            desc.vertexAttributes = rhiPoly3DPositionAttributes();
            desc.cull             = CullingMode::Front;
            desc.frontFace        = CullingFrontFace::CounterClockwise;
            desc.bHasDepthTest    = true;
            desc.bHasDepthWrite   = true;
            desc.depthCompare     = DepthCompare::Less;
            desc.bHasColor        = false;
            desc.bHasDepth        = true;
            desc.depthFormat      = RHI::ImageFormat::Depth32F;
            desc.pushConstantSize = sizeof(RHI::ShadowPush);
            desc.bHasPushVertex   = true;
            desc.layouts          = {m_layout};
            m_pipeline            = device->createPipeline(desc);
        }

        void LSceneShadow::onDestruction()
        {
            RHI::Device* device = m_backend->getRHIDevice();
            if (!device)
            {
                return;
            }

            for (RHI::BindGroup group : m_groups)
            {
                device->destroyBindGroup(group);
            }
            device->destroyPipeline(m_pipeline);
            device->destroyBindGroupLayout(m_layout);
        }

        bool LSceneShadow::onBeginRender(const Frame& inFrame)
        {
            return inFrame.hasFeature(RendererFeature::Fill) && inFrame.hasFeature(RendererFeature::Light) &&
                   inFrame.hasShadowCasterLights() && inFrame.hasShadowDraws();
        }

        void LSceneShadow::onRender(const Frame& inFrame, void* inData)
        {
            RHI::Frame*  rhi    = static_cast<RHI::Frame*>(inData);
            LScene*      parent = m_backend->getLayer<LScene>(SCENE_LAYER_ID);
            RHI::Device* device = m_backend->getRHIDevice();

            rhiReplaceGroup(
                device,
                m_groups,
                rhi->frameIndex,
                m_layout,
                {
                    {RHI_BINDING_LIGHT,          RHI::BindingType::StorageBuffer, rhi->lightBuffer,      {}, {}, 0},
                    {RHI_BINDING_MESH_INSTANCES, RHI::BindingType::StorageBuffer, rhi->instance3DBuffer, {}, {}, 0}
            }
            );

            for (std::uint32_t cascade = 0; cascade < SHADOW_CASCADE_COUNT; cascade++)
            {
                RHI::PassCreateInfo pass;
                pass.bHasDepth   = true;
                pass.depth.image = parent->shadowLayerViews[cascade];
                pass.depth.load  = RHI::LoadOp::Clear;
                pass.depth.store = RHI::StoreOp::Store;
                pass.width       = SHADOW_MAP_WIDTH;
                pass.height      = SHADOW_MAP_HEIGHT;
                rhi->commands->beginPass(pass);
                rhi->commands->setViewport(
                    {Vec2(static_cast<float>(SHADOW_MAP_WIDTH), static_cast<float>(SHADOW_MAP_HEIGHT))}
                );
                rhi->commands->setScissor({0, 0, SHADOW_MAP_WIDTH, SHADOW_MAP_HEIGHT});
                rhi->commands->setLineWidth(1.0f);
                rhi->commands->bindPipeline(m_pipeline);
                rhi->commands->bindGroup(0, m_groups[rhi->frameIndex]);
                RHI::ShadowPush push;
                push.cascade = cascade;
                rhi->commands->pushConstants(&push, sizeof(push));
                rhi->commands->bindVertexBuffer(parent->modelVertexBuffer);
                rhi->commands->bindIndexBuffer(parent->modelIndexBuffer);
                for (const DrawPoly& draw : inFrame.getShadowDraws())
                {
                    rhi->commands->drawIndexed(
                        draw.indexCount,
                        draw.instanceCount,
                        draw.indexStart,
                        draw.vertexStart,
                        draw.instanceStart
                    );
                }
                rhi->commands->endPass();
            }

            rhi->commands->prepareShaderRead(parent->shadowImage);
        }
    }
}
