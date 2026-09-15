#include "Chicane/Grid/Component/Window/Header.reflected.hpp"

#include "Chicane/Grid/Component/Window.hpp"

namespace Chicane
{
    namespace Grid
    {
        WindowHeader::WindowHeader(const pugi::xml_node& inNode)
            : Container(inNode),
              label(String::empty())
        {
            load("Assets/Engine/UI/Components/Window/Header.grid", "Assets/Engine/UI/Components/Window/Header.decal");
        }

        void WindowHeader::onTick(float inDeltaTime)
        {
            Container::onTick(inDeltaTime);

            refreshLabel();
        }

        void WindowHeader::onClose()
        {
            if (Window* window = Window::findFrom(this))
            {
                window->dismiss();
            }
        }

        void WindowHeader::refreshLabel()
        {
            label = parseText(getAttribute(LABEL_ATTRIBUTE_NAME)).trim();
        }
    }
}
