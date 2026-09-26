#include "Editor/UI/Component/Dock/Header.reflected.hpp"

#include <Chicane/Grid/Component/Dock/Panel.hpp>
#include <Chicane/Grid/Component/Window.hpp>

namespace Editor
{
    DockHeader::DockHeader(const Chicane::XmlNode& inNode)
        : Chicane::Grid::Container(inNode),
          label(Chicane::String::sEmpty()),
          pinState(PIN_STATE_UNPINNED_VALUE)
    {
        load(
            "Assets/Editor/UI/Components/Dock/Header/Index.grid",
            "Assets/Editor/UI/Components/Dock/Header/Index.decal"
        );
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

        if (Chicane::Grid::DockPanel* panel = Chicane::Grid::DockPanel::sFindFrom(this))
        {
            panel->setGrabbable(!isPinned());
        }
        else if (Chicane::Grid::Window* window = Chicane::Grid::Window::sFindFrom(this))
        {
            window->setGrabbable(!isPinned());
        }
    }

    void DockHeader::onClose()
    {
        if (Chicane::Grid::DockPanel* panel = Chicane::Grid::DockPanel::sFindFrom(this))
        {
            panel->addClassName("--closed");
        }
        else if (Chicane::Grid::Window* window = Chicane::Grid::Window::sFindFrom(this))
        {
            window->dismiss();
        }
    }

    void DockHeader::refreshLabel()
    {
        label = parseText(getAttribute(LABEL_ATTRIBUTE_NAME)).trim();
    }
}
