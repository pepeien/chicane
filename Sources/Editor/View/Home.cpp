#include "Editor/View/Home.reflected.hpp"

#include <Chicane/Core/FileSystem/File/Dialog.hpp>

#include <Chicane/Runtime/Application.hpp>

#include "Editor/Actor/Item.hpp"

namespace Editor
{
    HomeView::HomeView()
        : Chicane::Grid::View("Assets/Editor/Views/Home.grid"),
          telemetry(&Chicane::Application::getInstance().getTelemetry()),
          actors({}),
          folderItems(Chicane::FileSystem::ls())
    {
        Chicane::Application::getInstance().watchScene(
            [&](std::shared_ptr<Chicane::Scene> inScene)
            {
                if (!inScene)
                {
                    actors.clear();

                    return;
                }

                inScene->watchActors([&](std::vector<Chicane::Actor*> inActors) { actors = std::move(inActors); });
            }
        );
    }

    void HomeView::onAssetImport()
    {
        Chicane::FileSystem::FileDialog dialog;
        dialog.bCanSelectMany = false;
        dialog.location       = "/";
        dialog.title          = "Select asset source";
        dialog.addFilter("Fonts", {".bfon"});
        dialog.addFilter("Meshes", {".bmsh"});
        dialog.addFilter("Models", {".bmdl"});
        dialog.addFilter("Textures", {".btex"});
        dialog.addFilter("Skies", {".bsky"});
        dialog.addFilter("Sounds", {".bsnd"});

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
    }
}