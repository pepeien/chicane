#include "Chicane/Renderer/Layer/Scene/Foreground.hpp"

#include "Chicane/Renderer/Backend.hpp"
#include "Chicane/Renderer/Layer/Scene.hpp"
#include "Chicane/Renderer/Layer/Util.hpp"
#include "Chicane/Renderer/RHI/Mesh/Push.hpp"
#include "Chicane/Renderer/Shader/Bindings.hpp"

namespace Chicane
{
    namespace Renderer
    {
        LSceneForeground::LSceneForeground()
            : Layer(SCENE_FOREGROUND_LAYER_ID)
        {}

        void LSceneForeground::onInit()
        {
            RHI::Device* device = m_backend->getRHIDevice();

            RHI::BindGroupLayoutCreateInfo layout;
            RHI::Binding                   binding;
            binding.binding     = RHI_BINDING_CAMERA;
            binding.type        = RHI::BindingType::UniformBuffer;
            binding.bIsVertex   = true;
            binding.bIsFragment = false;
            layout.bindings.push_back(binding);
            binding.binding     = RHI_BINDING_LIGHT;
            binding.type        = RHI::BindingType::StorageBuffer;
            binding.bIsVertex   = true;
            binding.bIsFragment = true;
            layout.bindings.push_back(binding);
            binding.binding     = RHI_BINDING_MESH_INSTANCES;
            binding.type        = RHI::BindingType::StorageBuffer;
            binding.bIsVertex   = true;
            binding.bIsFragment = false;
            layout.bindings.push_back(binding);
            binding.binding     = RHI_BINDING_SHADOW_MAP;
            binding.type        = RHI::BindingType::SampledImage;
            binding.bIsVertex   = false;
            binding.bIsFragment = true;
            layout.bindings.push_back(binding);
            binding.binding     = RHI_BINDING_SKY_MAP;
            binding.type        = RHI::BindingType::SampledImage;
            binding.bIsVertex   = false;
            binding.bIsFragment = true;
            layout.bindings.push_back(binding);
            m_layout = device->createBindGroupLayout(layout);

            RHI::BindGroupLayoutCreateInfo textures;
            binding.binding           = RHI_BINDING_TEXTURES;
            binding.type              = RHI::BindingType::SampledImage;
            binding.count             = TEXTURE_SLOT_MAX;
            binding.bIsVertex         = false;
            binding.bIsFragment       = true;
            binding.bIsPartiallyBound = true;
            textures.bindings.push_back(binding);
            m_textureLayout = device->createBindGroupLayout(textures);

            RHI::PipelineCreateInfo desc;
            desc.vertexPath       = "Assets/Engine/Shaders/Scene/Mesh";
            desc.fragmentPath     = "Assets/Engine/Shaders/Scene/Mesh";
            desc.vertexStride     = sizeof(Vertex);
            desc.vertexAttributes = rhiPoly3DAttributes(true);
            desc.cull             = CullingMode::Back;
            desc.frontFace        = CullingFrontFace::CounterClockwise;
            desc.bHasDepthTest    = false;
            desc.bHasDepthWrite   = false;
            desc.blend            = RHI::BlendMode::Alpha;
            desc.bHasColor        = true;
            desc.bHasDepth        = true;
            desc.colorFormat      = device->sceneColorFormat();
            desc.depthFormat      = device->sceneDepthFormat();
            desc.pushConstantSize = sizeof(RHI::MeshPush);
            desc.bHasPushFragment = true;
            desc.layouts          = {m_layout, m_textureLayout};
            m_pipeline            = device->createPipeline(desc);
        }

        void LSceneForeground::onDestruction()
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
            device->destroyBindGroupLayout(m_textureLayout);
            device->destroyBindGroupLayout(m_layout);
        }

        bool LSceneForeground::onBeginRender(const Frame& inFrame)
        {
            return inFrame.hasForegroundDraws();
        }

        void LSceneForeground::onRender(const Frame& inFrame, void* inData)
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
                    {RHI_BINDING_CAMERA,         RHI::BindingType::UniformBuffer, rhi->cameraBuffer,     {},               {},                 0},
                    {RHI_BINDING_LIGHT,          RHI::BindingType::StorageBuffer, rhi->lightBuffer,      {},               {},                 0},
                    {RHI_BINDING_MESH_INSTANCES, RHI::BindingType::StorageBuffer, rhi->instance3DBuffer, {},               {},                 0},
                    {RHI_BINDING_SHADOW_MAP,
                     RHI::BindingType::SampledImage,
                     {},
                     parent->shadowImage,
                     parent->shadowSampler,
                     0                                                                                                                          },
                    {RHI_BINDING_SKY_MAP,        RHI::BindingType::SampledImage,  {},                    parent->skyImage, parent->skySampler, 0}
            }
            );

            rhi->commands->beginPass(rhiScenePass(*rhi, true, RHI::LoadOp::Load, RHI::LoadOp::Load));
            rhiApplyView(rhi->commands, m_backend, this);
            rhi->commands->bindPipeline(m_pipeline);
            rhi->commands->bindGroup(0, m_groups[rhi->frameIndex]);
            rhi->commands->bindGroup(1, rhi->textureTable);
            rhi->commands->bindVertexBuffer(parent->modelVertexBuffer);
            rhi->commands->bindIndexBuffer(parent->modelIndexBuffer);

            RHI::MeshPush push;
            push.transparentPass = -1;
            rhi->commands->pushConstants(&push, sizeof(push));

            for (const DrawPoly& draw : inFrame.getForegroundDraws())
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
    }
}
