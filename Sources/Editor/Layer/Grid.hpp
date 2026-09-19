#pragma once

#include <vector>

#include <Chicane/Renderer/Frame.hpp>
#include <Chicane/Renderer/Layer.hpp>
#include <Chicane/Renderer/RHI/BindGroup.hpp>
#include <Chicane/Renderer/RHI/BindGroup/Layout.hpp>
#include <Chicane/Renderer/RHI/Pipeline.hpp>

namespace Editor
{
    class LGrid : public Chicane::Renderer::Layer
    {
    public:
        LGrid();

    protected:
        void onInit() override;
        void onDestruction() override;

        bool onBeginRender(const Chicane::Renderer::Frame& inFrame) override;
        void onRender(const Chicane::Renderer::Frame& inFrame, void* inData = nullptr) override;

    private:
        void initViewport();

    private:
        Chicane::Renderer::RHI::Pipeline               m_pipeline;
        Chicane::Renderer::RHI::BindGroupLayout        m_layout;
        std::vector<Chicane::Renderer::RHI::BindGroup> m_groups;
    };
}
