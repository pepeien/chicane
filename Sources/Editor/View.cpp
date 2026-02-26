#include "View.hpp"

#include <Chicane/Core/FileSystem/File/Dialog.hpp>

#include <Chicane/Runtime/Application.hpp>

#include "Actor/Item.hpp"

namespace Editor
{
    View::View()
        : Chicane::Grid::View("Contents/Editor/Views/Home.grid"),
          m_telemetry(Chicane::Application::getInstance().getTelemetry()),
          m_uiFrameTime(Chicane::Reference::fromValue<const float>(&m_telemetry.delta)),
          m_uiFramesPerSecond(Chicane::Reference::fromValue<const std::uint32_t>(&m_telemetry.rate))
    {
        addReference("frameTime", &m_uiFrameTime);
        addReference("framesPerSecond", &m_uiFramesPerSecond);

        addFunction("onAssetImport", [this](const Chicane::Grid::Event& inEvent) { return onAssetImport(inEvent); });
    }

    Chicane::Reference View::onAssetImport(const Chicane::Grid::Event& inEvent)
    {
        Chicane::FileSystem::FileDialog dialog;
        dialog.bCanSelectMany = false;
        dialog.location       = "/";
        dialog.title          = "Select asset source";
        dialog.filters        = {
            {.title = "Fonts",    .extensions = {".bfon"}},
            {.title = "Meshes",   .extensions = {".bmsh"}},
            {.title = "Models",   .extensions = {".bmdl"}},
            {.title = "Textures", .extensions = {".btex"}},
            {.title = "Skies",    .extensions = {".bsky"}},
            {.title = "Sounds",   .extensions = {".bsnd"}},
        };
        dialog.open(
            [](const Chicane::FileSystem::Item::List& inFiles)
            {
                for (const Chicane::FileSystem::Item& item : inFiles)
                {
                    if (item.type != Chicane::FileSystem::ItemType::File)
                    {
                        continue;
                    }

                    if (item.extension.equals(Chicane::Box::Mesh::EXTENSION))
                    {
                        Chicane::Application::getInstance().getScene()->createActor<Item>(item.path);

                        continue;
                    }
                }
            }
        );

        return Chicane::Reference::empty();
    }
}