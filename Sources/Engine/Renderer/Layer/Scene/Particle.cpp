#include "Chicane/Renderer/Layer/Scene/Particle.hpp"

#include <algorithm>

#include "Chicane/Renderer/Backend.hpp"
#include "Chicane/Renderer/Layer/Util.hpp"
#include "Chicane/Renderer/Shader/Bindings.hpp"

namespace Chicane
{
    namespace Renderer
    {
        LSceneParticle::LSceneParticle()
            : Layer(SCENE_PARTICLE_LAYER_ID)
        {}

        void LSceneParticle::onInit()
        {
            RHI::Device* device = m_backend->getRHIDevice();

            RHI::BindGroupLayoutCreateInfo layout;
            RHI::Binding                   binding;
            binding.binding     = RHI_BINDING_CAMERA;
            binding.type        = RHI::BindingType::UniformBuffer;
            binding.bIsVertex   = true;
            binding.bIsFragment = false;
            layout.bindings.push_back(binding);
            binding.binding     = RHI_BINDING_PARTICLES;
            binding.type        = RHI::BindingType::StorageBuffer;
            binding.bIsVertex   = true;
            binding.bIsFragment = false;
            layout.bindings.push_back(binding);
            m_layout = device->createBindGroupLayout(layout);

            RHI::PipelineCreateInfo desc;
            desc.vertexPath     = "Assets/Engine/Shaders/Scene/Particle";
            desc.fragmentPath   = "Assets/Engine/Shaders/Scene/Particle";
            desc.topology       = RHI::PrimitiveTopology::TriangleStrip;
            desc.cull           = CullingMode::None;
            desc.bHasDepthTest  = true;
            desc.bHasDepthWrite = false;
            desc.depthCompare   = DepthCompare::Less;
            desc.blend          = RHI::BlendMode::Additive;
            desc.bHasColor      = true;
            desc.bHasDepth      = true;
            desc.colorFormat    = device->sceneColorFormat();
            desc.depthFormat    = device->sceneDepthFormat();
            desc.layouts        = {m_layout};
            m_pipeline          = device->createPipeline(desc);
        }

        void LSceneParticle::onDestruction()
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

        bool LSceneParticle::onBeginRender(const Frame& inFrame)
        {
            return inFrame.hasParticles();
        }

        void LSceneParticle::onRender(const Frame& inFrame, void* inData)
        {
            RHI::Frame*  rhi    = static_cast<RHI::Frame*>(inData);
            RHI::Device* device = m_backend->getRHIDevice();

            rhiReplaceGroup(
                device,
                m_groups,
                rhi->frameIndex,
                m_layout,
                {
                    {RHI_BINDING_CAMERA,    RHI::BindingType::UniformBuffer, rhi->cameraBuffer,   {}, {}, 0},
                    {RHI_BINDING_PARTICLES, RHI::BindingType::StorageBuffer, rhi->particleBuffer, {}, {}, 0}
            }
            );

            rhi->commands->beginPass(rhiScenePass(*rhi, true, RHI::LoadOp::Load, RHI::LoadOp::Load));
            rhiApplyView(rhi->commands, m_backend, this);
            rhi->commands->bindPipeline(m_pipeline);
            rhi->commands->bindGroup(0, m_groups[rhi->frameIndex]);
            const std::uint32_t count =
                std::min(static_cast<std::uint32_t>(inFrame.getParticles().size()), MAX_PARTICLES);
            rhi->commands->draw(4, count, 0, 0);
            rhi->commands->endPass();
        }
    }
}
