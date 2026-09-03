#include "Editor/UI/Component/Dock/Header.reflected.hpp"

#include <Chicane/Grid/Component/Dock/Panel.hpp>

namespace Editor
{
    DockHeader::DockHeader(const pugi::xml_node& inNode)
        : Chicane::Grid::Container(inNode),
          label(Chicane::String::empty()),
          pinState(PIN_STATE_UNPINNED_VALUE)
    {
        load("Assets/Editor/UI/Components/Dock/Header.grid", "Assets/Editor/UI/Components/Dock/Header.decal");
    }

    void DockHeader::onTick(float inDeltaTime)
    {
        Chicane::Grid::Container::onTick(inDeltaTime);

        refreshLabel();
    }

    bool DockHeader::isPinned() const
    {
        return pinState.equals(PIN_STATE_PINNED_VALUE);
    }

    void DockHeader::onPin()
    {
        pinState = isPinned() ? PIN_STATE_UNPINNED_VALUE : PIN_STATE_PINNED_VALUE;

        if (Chicane::Grid::DockPanel* panel = Chicane::Grid::DockPanel::findFrom(this))
        {
            panel->setGrabbable(!isPinned());
        }
    }

    void DockHeader::onClose()
    {
        if (Chicane::Grid::DockPanel* panel = Chicane::Grid::DockPanel::findFrom(this))
        {
            panel->addClassName("--closed");
        }
    }

    void DockHeader::refreshLabel()
    {
        label = parseText(getAttribute(LABEL_ATTRIBUTE_NAME)).trim();
    }
}
