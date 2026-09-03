#include "Editor/UI/View/Home.reflected.hpp"

#include <Chicane/Core/FileSystem/File/Dialog.hpp>
#include <Chicane/Core/FileSystem/Item/Type.hpp>
#include <Chicane/Runtime/Application.hpp>

#include "Editor/Actor/Item.hpp"
#include "Editor/UI/Component/Explorer.hpp"
#include "Editor/UI/Component/Header.hpp"
#include "Editor/UI/Component/Attributes.hpp"
#include "Editor/UI/Component/Outliner.hpp"
#include "Editor/UI/Component/Telemetry.hpp"

namespace Editor
{
    HomeView::HomeView()
        : Chicane::Grid::View(),
          outlinerActors({}),
          bIsItemSelected(false),
          selectedItem(nullptr),
          theme("dark")
    {
        import <Explorer>();
        import <Header>();
        import <Attributes>();
        import <Outliner>();
        import <Telemetry>();

        load("Assets/Editor/UI/Views/Home.grid", "Assets/Editor/UI/Views/Home.decal");

        Chicane::Application::getInstance().watchScene(
            [&](std::shared_ptr<Chicane::Scene> inScene)
            {
                if (!inScene)
                {
                    outlinerActors.clear();
                    onItemSelection(nullptr);

                    return;
                }

                inScene->watchActors(
                    [&](std::vector<Chicane::Actor*> inActors)
                    {
                        outlinerActors.clear();
                        outlinerActors = inActors;

                        if (!selectedItem)
                        {
                            return;
                        }

                        for (Chicane::Actor* actor : outlinerActors)
                        {
                            if (actor == selectedItem)
                            {
                                return;
                            }
                        }

                        onItemSelection(nullptr);
                    }
                );
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

    void HomeView::onThemeSwitch(Chicane::String inValue)
    {
        theme = inValue;
    }

    void HomeView::onItemSelection(Chicane::Object* inItem)
    {
        selectedItem    = !inItem ? nullptr : inItem;
        bIsItemSelected = selectedItem != nullptr;
    }
}
