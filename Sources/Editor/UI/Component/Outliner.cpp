#include "Editor/UI/Component/Outliner.reflected.hpp"

#include <Chicane/Core/Log.hpp>

#include "Editor/UI/Component/Outliner/Tab.hpp"
#include "Editor/UI/Component/Vec/Vec3.hpp"

namespace Editor
{
    Outliner::Outliner(const pugi::xml_node& inNode)
        : Chicane::Grid::Component(inNode),
          bIsItemSelected(false),
          selectedItem(nullptr)
    {
        import <OutlinerTab>();
        import <Vec3>();

        load("Assets/Editor/UI/Components/Outliner.grid", "Assets/Editor/UI/Components/Outliner.decal");
    }

    void Outliner::onItemSelection(Chicane::Object* inItem)
    {
        selectedItem = !inItem ? nullptr : inItem;

        bIsItemSelected = selectedItem != nullptr;
    }
}
