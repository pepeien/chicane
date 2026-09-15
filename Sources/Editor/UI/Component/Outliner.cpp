#include "Editor/UI/Component/Outliner.reflected.hpp"

#include "Editor/UI/Component/Dock/Header.hpp"
#include "Editor/UI/Prop.hpp"

namespace Editor
{
    Outliner::Outliner(const pugi::xml_node& inNode)
        : Chicane::Grid::Container(inNode),
          outlinerNodes({})
    {
        import <DockHeader>();

        load("Assets/Editor/UI/Components/Outliner.grid", "Assets/Editor/UI/Components/Outliner.decal");
    }

    void Outliner::onTick(float inDeltaTime)
    {
        Chicane::Grid::Container::onTick(inDeltaTime);

        Prop::copy(this, NODES_ATTRIBUTE, outlinerNodes);
    }

    void Outliner::onItemSelection(Chicane::Object* inItem)
    {
        Prop::invoke(this, ON_ITEM_SELECTION_ATTRIBUTE, inItem);
    }

    void Outliner::onItemToggle(Chicane::Object* inItem)
    {
        Prop::invoke(this, ON_ITEM_TOGGLE_ATTRIBUTE, inItem);
    }
}
