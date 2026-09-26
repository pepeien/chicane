#include "Editor/Layer/Icon.hpp"
#include "Editor/Layer/Icon/Push.hpp"

#include <algorithm>
#include <cmath>

#include <Chicane/Box.hpp>
#include <Chicane/Box/Texture.hpp>
#include <Chicane/Renderer/Backend.hpp>
#include <Chicane/Renderer/Draw/Texture/Data.hpp>
#include <Chicane/Renderer/Instance.hpp>
#include <Chicane/Renderer/Layer/Util.hpp>
#include <Chicane/Renderer/Light/Type.hpp>
#include <Chicane/Renderer/Shader/Bindings.hpp>
#include <Chicane/Runtime/Instance.hpp>
#include <Chicane/Runtime/Scene.hpp>
#include <Chicane/Runtime/Scene/Component/Camera.hpp>
#include <Chicane/Runtime/Scene/Component/Sound.hpp>

#include "Editor/Viewer/Scene.hpp"

namespace Editor
{
    static constexpr inline const char* ICON_LIGHT_POINT       = "Editor_Icon_Light_Point";
    static constexpr inline const char* ICON_LIGHT_SPOT        = "Editor_Icon_Light_Spot";
    static constexpr inline const char* ICON_LIGHT_DIRECTIONAL = "Editor_Icon_Light_Directional";
    static constexpr inline const char* ICON_LIGHT_ENVIRONMENT = "Editor_Icon_Light_Environment";
    static constexpr inline const char* ICON_SOUND             = "Editor_Icon_Sound";

    static constexpr inline const char* PATH_LIGHT_POINT       = "Assets/Editor/Textures/Icons/Light/Point.btex";
    static constexpr inline const char* PATH_LIGHT_SPOT        = "Assets/Editor/Textures/Icons/Light/Spot.btex";
    static constexpr inline const char* PATH_LIGHT_DIRECTIONAL = "Assets/Editor/Textures/Icons/Light/Directional.btex";
    static constexpr inline const char* PATH_LIGHT_ENVIRONMENT = "Assets/Editor/Textures/Icons/Light/Environment.btex";
    static constexpr inline const char* PATH_SOUND             = "Assets/Editor/Textures/Icons/Sound.btex";

    static constexpr inline float ICON_SIZE_SCALE = 0.045f;
    static constexpr inline float ICON_SIZE_MIN   = 0.18f;
    static constexpr inline float ICON_SIZE_MAX   = 0.85f;

    static void uploadTexture(const Chicane::Box::Texture* inTexture)
    {
        if (!inTexture)
        {
            return;
        }

        Chicane::Renderer::Instance* renderer = Chicane::Instance::sInstance().getRenderer();
        if (!renderer)
        {
            return;
        }

        Chicane::Renderer::DrawTextureData data;
        data.reference   = inTexture->getId();
        data.image       = inTexture->getFrame(0).lock();
        data.mips        = inTexture->getMipChain(0);
        data.bStreamable = false;
        renderer->loadTexture(data);
    }

    LIcon::LIcon()
        : Layer("Editor_Scene_Icon")
    {}

    void LIcon::onInit()
    {
        Chicane::Renderer::RHI::Device* device = m_backend->getRHIDevice();

        Chicane::Renderer::RHI::BindGroupLayoutCreateInfo layout;
        Chicane::Renderer::RHI::Binding                   binding;
        binding.binding     = Chicane::Renderer::RHI_BINDING_CAMERA;
        binding.type        = Chicane::Renderer::RHI::BindingType::UniformBuffer;
        binding.bIsVertex   = true;
        binding.bIsFragment = false;
        layout.bindings.push_back(binding);
        m_layout = device->createBindGroupLayout(layout);

        Chicane::Renderer::RHI::BindGroupLayoutCreateInfo textures;
        binding.binding           = Chicane::Renderer::RHI_BINDING_TEXTURES;
        binding.type              = Chicane::Renderer::RHI::BindingType::SampledImage;
        binding.count             = Chicane::Renderer::TEXTURE_SLOT_MAX;
        binding.bIsVertex         = false;
        binding.bIsFragment       = true;
        binding.bIsPartiallyBound = true;
        textures.bindings.push_back(binding);
        m_textureLayout = device->createBindGroupLayout(textures);

        Chicane::Renderer::RHI::PipelineCreateInfo desc;
        desc.vertexPath       = "Assets/Editor/Shaders/Icon";
        desc.fragmentPath     = "Assets/Editor/Shaders/Icon";
        desc.topology         = Chicane::Renderer::RHI::PrimitiveTopology::TriangleStrip;
        desc.cull             = Chicane::Renderer::CullingMode::None;
        desc.bHasDepthTest    = true;
        desc.bHasDepthWrite   = false;
        desc.depthCompare     = Chicane::Renderer::DepthCompare::LessOrEqual;
        desc.blend            = Chicane::Renderer::RHI::BlendMode::Alpha;
        desc.bHasColor        = true;
        desc.bHasDepth        = true;
        desc.colorFormat      = device->sceneColorFormat();
        desc.depthFormat      = device->sceneDepthFormat();
        desc.pushConstantSize = sizeof(IconPush);
        desc.bHasPushVertex   = true;
        desc.bHasPushFragment = true;
        desc.layouts          = {m_layout, m_textureLayout};
        m_pipeline            = device->createPipeline(desc);

        Chicane::Renderer::ViewportSettings viewport;
        viewport.width  = "100%";
        viewport.height = "100%";
        setViewport(viewport);

        loadTextures();
    }

    void LIcon::onDestruction()
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
        device->destroyBindGroupLayout(m_textureLayout);
        device->destroyBindGroupLayout(m_layout);
    }

    bool LIcon::onBeginRender(const Chicane::Renderer::Frame& inFrame)
    {
        (void)inFrame;

        return dynamic_cast<ViewerScene*>(Chicane::Instance::sInstance().getScene().get()) == nullptr;
    }

    void LIcon::onRender(const Chicane::Renderer::Frame& inFrame, void* inData)
    {
        (void)inFrame;

        Chicane::Scene* scene = Chicane::Instance::sInstance().getScene().get();
        if (!scene)
        {
            return;
        }

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
        rhi->commands->bindGroup(1, rhi->textureTable);

        const Chicane::Renderer::Draw::Id point       = findIcon(ICON_LIGHT_POINT);
        const Chicane::Renderer::Draw::Id spot        = findIcon(ICON_LIGHT_SPOT);
        const Chicane::Renderer::Draw::Id directional = findIcon(ICON_LIGHT_DIRECTIONAL);
        const Chicane::Renderer::Draw::Id environment = findIcon(ICON_LIGHT_ENVIRONMENT);
        const Chicane::Renderer::Draw::Id sound       = findIcon(ICON_SOUND);

        for (Chicane::CLight* light : scene->getComponents<Chicane::CLight>())
        {
            if (!shouldDraw(light))
            {
                continue;
            }

            Chicane::Renderer::Draw::Id texture = point;
            switch (light->getType())
            {
            case Chicane::LightType::Spot:
                texture = spot;
                break;

            case Chicane::LightType::Directional:
                texture = directional;
                break;

            case Chicane::LightType::Environment:
                texture = environment;
                break;

            default:
                break;
            }

            const Chicane::Vec3 tint = light->getColor().max(Chicane::Vec3(0.2f));
            drawIcon(rhi, light->getTranslation(), iconSize(light), Chicane::Vec4(tint, 1.0f), texture);
        }

        for (Chicane::CSound* component : scene->getComponents<Chicane::CSound>())
        {
            if (!shouldDraw(component))
            {
                continue;
            }

            drawIcon(rhi, component->getTranslation(), iconSize(component), Chicane::Vec4(1.0f), sound);
        }

        rhi->commands->endPass();
    }

    void LIcon::loadTextures()
    {
        uploadTexture(Chicane::Box::load<Chicane::Box::Texture>(PATH_LIGHT_POINT));
        uploadTexture(Chicane::Box::load<Chicane::Box::Texture>(PATH_LIGHT_SPOT));
        uploadTexture(Chicane::Box::load<Chicane::Box::Texture>(PATH_LIGHT_DIRECTIONAL));
        uploadTexture(Chicane::Box::load<Chicane::Box::Texture>(PATH_LIGHT_ENVIRONMENT));
        uploadTexture(Chicane::Box::load<Chicane::Box::Texture>(PATH_SOUND));
    }

    Chicane::Renderer::Draw::Id LIcon::findIcon(const Chicane::String& inReference) const
    {
        Chicane::Renderer::Instance* renderer = Chicane::Instance::sInstance().getRenderer();
        if (!renderer)
        {
            return Chicane::Renderer::Draw::InvalidId;
        }

        return renderer->findTexture(inReference);
    }

    bool LIcon::shouldDraw(const Chicane::Component* inComponent) const
    {
        if (!inComponent || inComponent->isTransient())
        {
            return false;
        }

        if (Chicane::Object* parent = inComponent->getParent())
        {
            return !parent->isTransient();
        }

        return true;
    }

    float LIcon::iconSize(const Chicane::Object* inComponent) const
    {
        Chicane::Scene* scene = Chicane::Instance::sInstance().getScene().get();
        if (!scene || !inComponent)
        {
            return ICON_SIZE_MIN;
        }

        const std::vector<Chicane::CCamera*> cameras = scene->getActiveComponents<Chicane::CCamera>();
        if (cameras.empty() || !cameras.back())
        {
            return ICON_SIZE_MIN;
        }

        const Chicane::Vec3 delta    = inComponent->getTranslation() - cameras.back()->getTranslation();
        const float         distance = std::sqrt(std::max(delta.dot(delta), 0.0f));

        return std::clamp(distance * ICON_SIZE_SCALE, ICON_SIZE_MIN, ICON_SIZE_MAX);
    }

    void LIcon::drawIcon(
        Chicane::Renderer::RHI::Frame* inFrame,
        const Chicane::Vec3&           inPosition,
        float                          inSize,
        const Chicane::Vec4&           inColor,
        Chicane::Renderer::Draw::Id    inTexture
    ) const
    {
        if (!inFrame || inTexture <= Chicane::Renderer::Draw::InvalidId)
        {
            return;
        }

        IconPush push;
        push.positionSize[0] = inPosition.x;
        push.positionSize[1] = inPosition.y;
        push.positionSize[2] = inPosition.z;
        push.positionSize[3] = inSize;
        push.color[0]        = inColor.x;
        push.color[1]        = inColor.y;
        push.color[2]        = inColor.z;
        push.color[3]        = inColor.w;
        push.extra[0]        = inTexture;

        inFrame->commands->pushConstants(&push, sizeof(push));
        inFrame->commands->draw(4, 1, 0, 0);
    }
}
