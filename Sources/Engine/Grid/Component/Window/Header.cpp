#include "Chicane/Grid/Component/Window/Header.reflected.hpp"

#include "Chicane/Grid/Component/Window.hpp"

namespace Chicane
{
    namespace Grid
    {
        WindowHeader::WindowHeader(const XmlNode& inNode)
            : Container(inNode),
              label(String::empty())
        {
            load("Assets/Engine/UI/Components/Window/Header.grid", "Assets/Engine/UI/Components/Window/Header.decal");

            watchAttribute(
                LABEL_ATTRIBUTE_NAME,
                [this](const String& inValue)
                {
                    label = parseText(inValue).trim();
                }
            );
        }

        void WindowHeader::onClose()
        {
            if (Window* window = Window::findFrom(this))
            {
                window->dismiss();
            }
        }
    }
}
