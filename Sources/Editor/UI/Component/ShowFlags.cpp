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
          meshesState("idle"),
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
        if (Chicane::Renderer::Instance* renderer = getRenderer())
        {
            renderer->toggleFill();
        }
    }

    void ShowFlags::onMeshesToggle()
    {
        toggleDebug(Chicane::Renderer::DebugMode::Meshes);
    }

    void ShowFlags::onBoundsToggle()
    {
        toggleDebug(Chicane::Renderer::DebugMode::Bounds);
    }

    void ShowFlags::onTracesToggle()
    {
        toggleDebug(Chicane::Renderer::DebugMode::Traces);
    }

    void ShowFlags::onCollidersToggle()
    {
        toggleDebug(Chicane::Renderer::DebugMode::Colliders);
    }

    void ShowFlags::onSkeletonsToggle()
    {
        toggleDebug(Chicane::Renderer::DebugMode::Skeletons);
    }

    Chicane::Renderer::Instance* ShowFlags::getRenderer() const
    {
        return Chicane::Application::getInstance().getRenderer();
    }

    void ShowFlags::syncFlags()
    {
        const Chicane::Renderer::Instance* renderer = getRenderer();

        fillState      = flagState(renderer && renderer->hasFill());
        meshesState    = flagState(renderer && renderer->hasDebug(Chicane::Renderer::DebugMode::Meshes));
        boundsState    = flagState(renderer && renderer->hasDebug(Chicane::Renderer::DebugMode::Bounds));
        tracesState    = flagState(renderer && renderer->hasDebug(Chicane::Renderer::DebugMode::Traces));
        collidersState = flagState(renderer && renderer->hasDebug(Chicane::Renderer::DebugMode::Colliders));
        skeletonsState = flagState(renderer && renderer->hasDebug(Chicane::Renderer::DebugMode::Skeletons));
    }

    void ShowFlags::toggleDebug(Chicane::Renderer::DebugMode inMode)
    {
        if (Chicane::Renderer::Instance* renderer = getRenderer())
        {
            renderer->toggleDebug(inMode);
        }
    }
}
