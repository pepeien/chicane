#include "Editor/UI/Component/Explorer/Item.reflected.hpp"

#include <Chicane/Core/FileSystem/Item.hpp>
#include <Chicane/Core/FileSystem/Item/Type.hpp>
#include <Chicane/Core/Reflection/Type/Field/Acessor.hpp>

namespace Editor
{
    ExplorerItem::ExplorerItem(const pugi::xml_node& inNode)
        : Chicane::Grid::Button(inNode),
          isFolder(false),
          isFile(true),
          typeClass("file"),
          selectionState("idle"),
          itemName(Chicane::String::empty()),
          itemPath(Chicane::String::empty())
    {
        load("Assets/Editor/UI/Components/Explorer/Item.grid", "Assets/Editor/UI/Components/Explorer/Item.decal");
    }

    void ExplorerItem::onTick(float inDeltaTime)
    {
        Chicane::Grid::Button::onTick(inDeltaTime);

        refreshState();
    }

    void ExplorerItem::refreshState()
    {
        selectionState = "idle";
        isFolder       = false;
        isFile         = true;
        typeClass      = "file";
        itemName       = Chicane::String::empty();
        itemPath       = Chicane::String::empty();

        const Chicane::ReflectionFieldAccessor accessor = getField("child");
        if (!accessor.isValid())
        {
            return;
        }

        const void* instance =
            accessor.boundInstance != nullptr ? accessor.boundInstance : static_cast<const void*>(this);
        const Chicane::FileSystem::Item* item = accessor.getValue<Chicane::FileSystem::Item>(instance);
        if (!item)
        {
            return;
        }

        itemName  = item->name;
        itemPath  = item->path.toString();
        isFolder  = item->type == Chicane::FileSystem::ItemType::Folder;
        isFile    = item->type == Chicane::FileSystem::ItemType::File;
        typeClass = isFolder ? "folder" : "file";

        const Chicane::String selected = parseText("{{ selectedAssetName }}");
        if (!selected.isEmpty() && selected.equals(item->name))
        {
            selectionState = "selected";
        }
    }
}
