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

        {
            Chicane::Grid::DockPanel* panel = Chicane::Grid::DockPanel::sFindFrom(this);

            Chicane::Grid::Window* window = Chicane::Grid::Window::sFindFrom(this);

            const bool bHasPanel = static_cast<bool>(panel);

            if (bHasPanel)
            {
                panel->setGrabbable(!isPinned());
            }

            const bool bHasWindow = !bHasPanel && (window);

            if (bHasWindow)
            {
                window->setGrabbable(!isPinned());
            }
        }
    }

    void DockHeader::onClose()
    {
        {
            Chicane::Grid::DockPanel* panel = Chicane::Grid::DockPanel::sFindFrom(this);

            Chicane::Grid::Window* window = Chicane::Grid::Window::sFindFrom(this);

            const bool bHasPanel = static_cast<bool>(panel);

            if (bHasPanel)
            {
                panel->classList.add("--closed");
            }

            const bool bHasWindow = !bHasPanel && (window);

            if (bHasWindow)
            {
                window->dismiss();
            }
        }
    }

    void DockHeader::refreshLabel()
    {
        label = parseText(getAttribute(LABEL_ATTRIBUTE_NAME)).trim();
    }
}
