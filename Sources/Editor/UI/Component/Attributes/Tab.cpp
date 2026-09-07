#include "Editor/UI/Component/Attributes/Tab.reflected.hpp"

namespace Editor
{
    AttributesTab::AttributesTab(const pugi::xml_node& inNode)
        : Chicane::Grid::Component(inNode),
          contentVisibility("hidden"),
          label(Chicane::String::empty())
    {
        load("Assets/Editor/UI/Components/Attributes/Tab.grid", "Assets/Editor/UI/Components/Attributes/Tab.decal");
    }

    void AttributesTab::onTick(float inDeltaTime)
    {
        Chicane::Grid::Component::onTick(inDeltaTime);

        refreshLabel();
    }

    void AttributesTab::onDropdownClick()
    {
        contentVisibility =
            contentVisibility.equals(VISILITY_STATUS_VISIBLE) ? VISILITY_STATUS_HIDDEN : VISILITY_STATUS_VISIBLE;
    }

    void AttributesTab::refreshLabel()
    {
        label = parseText(getAttribute(LABEL_ATTRIBUTE_NAME)).trim();
    }
}
