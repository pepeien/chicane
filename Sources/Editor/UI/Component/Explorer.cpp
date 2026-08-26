#include "Editor/UI/Component/Explorer.reflected.hpp"

#include <algorithm>

#include <Chicane/Core/FileSystem/Item/Type.hpp>

#include "Editor/UI/Component/Explorer/Item.hpp"

namespace Editor
{
    Explorer::Explorer(const pugi::xml_node& inNode)
        : Chicane::Grid::Component(inNode),
          explorerFolder({})
    {
        import <ExplorerItem>();

        load("Assets/Editor/UI/Components/Explorer.grid", "Assets/Editor/UI/Components/Explorer.decal");

        explorerFolder.children = Chicane::FileSystem::ls();
    }

    std::vector<Chicane::String> Explorer::getFolderLocations() const
    {
        return std::vector<Chicane::String>(explorerFolder.path.begin(), explorerFolder.path.end());
    }

    void Explorer::onOutlinerSwitch(Chicane::String inFolderName)
    {
        if (inFolderName.isEmpty())
        {
            return;
        }

        const auto found = std::find_if(
            explorerFolder.children.begin(),
            explorerFolder.children.end(),
            [&](const Chicane::FileSystem::Item& inItem) { return inItem.name == inFolderName; }
        );

        if (found == explorerFolder.children.end() || found->type != Chicane::FileSystem::ItemType::Folder)
        {
            return;
        }

        explorerFolder.path /= inFolderName;
        explorerFolder.children = Chicane::FileSystem::ls(explorerFolder.path);
    }
}
