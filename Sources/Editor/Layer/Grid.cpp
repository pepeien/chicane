#include "Editor/Layer/Grid.hpp"
#include "Editor/Layer/Grid/Push.hpp"

#include <algorithm>

#include <Chicane/Renderer/Backend.hpp>
#include <Chicane/Renderer/Layer/Util.hpp>
#include <Chicane/Renderer/Shader/Bindings.hpp>
#include <Chicane/Runtime/Application.hpp>

#include "Editor/Viewer/Scene.hpp"
#include "Editor/Viewport/Overlay.hpp"

namespace Editor
{
    LGrid::LGrid()
        : Layer("Editor_Scene_Grid")
    {}

    void LGrid::onInit()
    {
        Chicane::Renderer::RHI::Device* device = m_backend->getRHIDevice();

        Chicane::Renderer::RHI::BindGroupLayoutCreateInfo layout;
        Chicane::Renderer::RHI::Binding                   binding;
        binding.binding     = Chicane::Renderer::RHI_BINDING_CAMERA;
        binding.type        = Chicane::Renderer::RHI::BindingType::UniformBuffer;
        binding.bIsVertex   = true;
        binding.bIsFragment = true;
        layout.bindings.push_back(binding);
        m_layout = device->createBindGroupLayout(layout);

        Chicane::Renderer::RHI::PipelineCreateInfo desc;
        desc.vertexPath       = "Assets/Editor/Shaders/Grid";
        desc.fragmentPath     = "Assets/Editor/Shaders/Grid";
        desc.cull             = Chicane::Renderer::CullingMode::Back;
        desc.frontFace        = Chicane::Renderer::CullingFrontFace::CounterClockwise;
        desc.bHasDepthTest    = true;
        desc.bHasDepthWrite   = false;
        desc.depthCompare     = Chicane::Renderer::DepthCompare::LessOrEqual;
        desc.blend            = Chicane::Renderer::RHI::BlendMode::Alpha;
        desc.bHasColor        = true;
        desc.bHasDepth        = true;
        desc.colorFormat      = device->sceneColorFormat();
        desc.depthFormat      = device->sceneDepthFormat();
        desc.pushConstantSize = sizeof(GridPush);
        desc.bHasPushFragment = true;
        desc.layouts          = {m_layout};
        m_pipeline            = device->createPipeline(desc);

        initViewport();
    }

    void LGrid::onDestruction()
    {
        Chicane::Renderer::RHI::Device* device = m_backend->getRHIDevice();
        if (!device)
        {
            return;
        }

        for (Chicane::Renderer::RHI::BindGroup group : m_groups)
        {
            device->destroyBindGroup(group);
        }
        device->destroyPipeline(m_pipeline);
        device->destroyBindGroupLayout(m_layout);
    }

    bool LGrid::onBeginRender(const Chicane::Renderer::Frame& inFrame)
    {
        (void)inFrame;

        if (!ViewportOverlay::getInstance().bGridEnabled)
        {
            return false;
        }

        return dynamic_cast<ViewerScene*>(Chicane::Application::getInstance().getScene().get()) == nullptr;
    }

    void LGrid::onRender(const Chicane::Renderer::Frame& inFrame, void* inData)
    {
        (void)inFrame;
        auto* rhi    = static_cast<Chicane::Renderer::RHI::Frame*>(inData);
        auto* device = m_backend->getRHIDevice();

        Chicane::Renderer::rhiReplaceGroup(
            device,
            m_groups,
            rhi->frameIndex,
            m_layout,
            {
                {Chicane::Renderer::RHI_BINDING_CAMERA,
                 Chicane::Renderer::RHI::BindingType::UniformBuffer,
                 rhi->cameraBuffer,
                 {},
                 {},
                 0}
        }
        );

        const ViewportOverlay& overlay = ViewportOverlay::getInstance();
        GridPush               push;
        push.color[0] = overlay.gridColor.x;
        push.color[1] = overlay.gridColor.y;
        push.color[2] = overlay.gridColor.z;
        push.color[3] = 1.0f;
        push.cells[0] = std::max(overlay.gridScale, 0.001f);
        push.cells[1] = std::max(overlay.gridScale, 0.001f) * std::max(overlay.gridDivisions, 1.0f);
        push.cells[2] = overlay.bGridAxisX ? 1.0f : 0.0f;
        push.cells[3] = overlay.bGridAxisY ? 1.0f : 0.0f;
        push.extra[0] = overlay.bGridAxisZ ? 1.0f : 0.0f;

        rhi->commands->beginPass(
            Chicane::Renderer::rhiScenePass(
                *rhi,
                true,
                Chicane::Renderer::RHI::LoadOp::Load,
                Chicane::Renderer::RHI::LoadOp::Load
            )
        );
        Chicane::Renderer::rhiApplyView(rhi->commands, m_backend, this);
        rhi->commands->bindPipeline(m_pipeline);
        rhi->commands->bindGroup(0, m_groups[rhi->frameIndex]);
        rhi->commands->pushConstants(&push, sizeof(push));
        rhi->commands->draw(6, 1, 0, 0);
        rhi->commands->endPass();
    }

    void LGrid::initViewport()
    {
        Chicane::Renderer::ViewportSettings viewport;
        viewport.width  = "100%";
        viewport.height = "100%";

        setViewport(viewport);

        for (Layer* layer :
             m_backend->findLayers([](const Layer* inLayer)
                                   { return inLayer->getId().contains(Chicane::Renderer::SCENE_LAYER_ID); }))
        {
            layer->setViewport(viewport);
        }
    }
}
