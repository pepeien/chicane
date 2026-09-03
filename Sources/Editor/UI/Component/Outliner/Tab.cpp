#include "Editor/UI/Component/Outliner/Tab.reflected.hpp"

namespace Editor
{
    OutlinerTab::OutlinerTab(const pugi::xml_node& inNode)
        : Chicane::Grid::Component(inNode),
          contentVisibility("hidden"),
          label(Chicane::String::empty())
    {
        load("Assets/Editor/UI/Components/Outliner/Tab.grid", "Assets/Editor/UI/Components/Outliner/Tab.decal");
    }

    void OutlinerTab::onTick(float inDeltaTime)
    {
        Chicane::Grid::Component::onTick(inDeltaTime);

        refreshLabel();
    }

    void OutlinerTab::onDropdownClick()
    {
        contentVisibility =
            contentVisibility.equals(VISILITY_STATUS_VISIBLE) ? VISILITY_STATUS_HIDDEN : VISILITY_STATUS_VISIBLE;
    }

    void OutlinerTab::refreshLabel()
    {
        label = parseText(getAttribute(LABEL_ATTRIBUTE_NAME)).trim();
    }
}
