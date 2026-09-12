#pragma once

#include <Chicane/Core/Reflection.hpp>
#include <Chicane/Core/String.hpp>
#include <Chicane/Core/Xml.hpp>
#include <Chicane/Grid/Component/Container.hpp>
#include <Chicane/Renderer/Feature.hpp>
#include <Chicane/Renderer/Instance.hpp>

namespace Editor
{
    CH_TYPE(Type = (Manual), Alias = (Editor::ShowFlags))
    class ShowFlags : public Chicane::Grid::Container
    {
    public:
        CH_CONSTRUCTOR()
        ShowFlags(const pugi::xml_node& inNode);

    protected:
        void onTick(float inDeltaTime) override;

    public:
        CH_FUNCTION()
        void onFillToggle();

        CH_FUNCTION()
        void onWireframeToggle();

        CH_FUNCTION()
        void onBoundsToggle();

        CH_FUNCTION()
        void onTracesToggle();

        CH_FUNCTION()
        void onCollidersToggle();

        CH_FUNCTION()
        void onSkeletonsToggle();

    private:
        Chicane::Renderer::Instance* getRenderer() const;
        void syncFlags();
        void toggleFeature(Chicane::Renderer::RendererFeature inFeature);

    public:
        CH_FIELD()
        Chicane::String fillState;
        CH_FIELD()
        Chicane::String wireframeState;
        CH_FIELD()
        Chicane::String boundsState;
        CH_FIELD()
        Chicane::String tracesState;
        CH_FIELD()
        Chicane::String collidersState;
        CH_FIELD()
        Chicane::String skeletonsState;
    };
}
