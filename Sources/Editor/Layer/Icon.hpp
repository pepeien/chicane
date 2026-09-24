#pragma once

#include <vector>

#include <Chicane/Core/Math/Vec/Vec3.hpp>
#include <Chicane/Core/Math/Vec/Vec4.hpp>
#include <Chicane/Renderer/Draw.hpp>
#include <Chicane/Renderer/Frame.hpp>
#include <Chicane/Renderer/Layer.hpp>
#include <Chicane/Renderer/RHI/BindGroup.hpp>
#include <Chicane/Renderer/RHI/BindGroup/Layout.hpp>
#include <Chicane/Renderer/RHI/Frame.hpp>
#include <Chicane/Renderer/RHI/Pipeline.hpp>
#include <Chicane/Runtime/Scene/Component.hpp>
#include <Chicane/Runtime/Scene/Component/Light.hpp>
#include <Chicane/Runtime/Scene/Object.hpp>

namespace Editor
{
    class LIcon : public Chicane::Renderer::Layer
    {
    public:
        LIcon();

    protected:
        void onInit() override;
        void onDestruction() override;

        bool onBeginRender(const Chicane::Renderer::Frame& inFrame) override;
        void onRender(const Chicane::Renderer::Frame& inFrame, void* inData = nullptr) override;

    private:
        void loadTextures();
        Chicane::Renderer::Draw::Id findIcon(const Chicane::String& inReference) const;
        bool shouldDraw(const Chicane::Component* inComponent) const;
        float iconSize(const Chicane::Object* inComponent) const;
        void drawIcon(
            Chicane::Renderer::RHI::Frame* inFrame,
            const Chicane::Vec3&           inPosition,
            float                          inSize,
            const Chicane::Vec4&           inColor,
            Chicane::Renderer::Draw::Id    inTexture
        ) const;

    private:
        Chicane::Renderer::RHI::Pipeline               m_pipeline;
        Chicane::Renderer::RHI::BindGroupLayout        m_layout;
        Chicane::Renderer::RHI::BindGroupLayout        m_textureLayout;
        std::vector<Chicane::Renderer::RHI::BindGroup> m_groups;
    };
}
