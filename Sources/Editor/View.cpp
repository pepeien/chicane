#include "Editor/View.hpp"

#include <Chicane/Core/FileSystem/File/Dialog.hpp>

#include <Chicane/Runtime/Application.hpp>

#include "Editor/Actor/Item.hpp"

namespace Editor
{
    View::View()
        : Chicane::Grid::View("Contents/Editor/Views/Home.grid"),
          telemetry(Chicane::Application::getInstance().getTelemetry())
    {}

    Chicane::Reference View::onAssetImport(const Chicane::Grid::Event& inEvent)
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

        return Chicane::Reference::empty();
    }
}