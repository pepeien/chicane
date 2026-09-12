#include "Editor/UI/Component/ShowFlags.reflected.hpp"

#include <Chicane/Runtime/Application.hpp>

namespace Editor
{
    namespace
    {
        Chicane::String flagState(bool inValue)
        {
            return inValue ? "checked" : "idle";
        }
    }

    ShowFlags::ShowFlags(const pugi::xml_node& inNode)
        : Chicane::Grid::Container(inNode),
          fillState("idle"),
          wireframeState("idle"),
          boundsState("idle"),
          tracesState("idle"),
          collidersState("idle"),
          skeletonsState("idle")
    {
        load("Assets/Editor/UI/Components/ShowFlags.grid", "Assets/Editor/UI/Components/ShowFlags.decal");
    }

    void ShowFlags::onTick(float inDeltaTime)
    {
        Chicane::Grid::Container::onTick(inDeltaTime);

        syncFlags();
    }

    void ShowFlags::onFillToggle()
    {
        toggleFeature(Chicane::Renderer::RendererFeature::Fill);
    }

    void ShowFlags::onWireframeToggle()
    {
        toggleFeature(Chicane::Renderer::RendererFeature::Wireframe);
    }

    void ShowFlags::onBoundsToggle()
    {
        toggleFeature(Chicane::Renderer::RendererFeature::Bounds);
    }

    void ShowFlags::onTracesToggle()
    {
        toggleFeature(Chicane::Renderer::RendererFeature::Traces);
    }

    void ShowFlags::onCollidersToggle()
    {
        toggleFeature(Chicane::Renderer::RendererFeature::Colliders);
    }

    void ShowFlags::onSkeletonsToggle()
    {
        toggleFeature(Chicane::Renderer::RendererFeature::Skeletons);
    }

    Chicane::Renderer::Instance* ShowFlags::getRenderer() const
    {
        return Chicane::Application::getInstance().getRenderer();
    }

    void ShowFlags::syncFlags()
    {
        const Chicane::Renderer::Instance* renderer = getRenderer();

        fillState      = flagState(renderer && renderer->hasFeature(Chicane::Renderer::RendererFeature::Fill));
        wireframeState = flagState(renderer && renderer->hasFeature(Chicane::Renderer::RendererFeature::Wireframe));
        boundsState    = flagState(renderer && renderer->hasFeature(Chicane::Renderer::RendererFeature::Bounds));
        tracesState    = flagState(renderer && renderer->hasFeature(Chicane::Renderer::RendererFeature::Traces));
        collidersState = flagState(renderer && renderer->hasFeature(Chicane::Renderer::RendererFeature::Colliders));
        skeletonsState = flagState(renderer && renderer->hasFeature(Chicane::Renderer::RendererFeature::Skeletons));
    }

    void ShowFlags::toggleFeature(Chicane::Renderer::RendererFeature inFeature)
    {
        if (Chicane::Renderer::Instance* renderer = getRenderer())
        {
            renderer->toggleFeature(inFeature);
        }
    }
}
