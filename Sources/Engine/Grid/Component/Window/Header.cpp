#include "Chicane/Grid/Component/Window/Header.reflected.hpp"

#include "Chicane/Grid/Component/Window.hpp"

namespace Chicane
{
    namespace Grid
    {
        WindowHeader::WindowHeader(const XmlNode& inNode)
            : Container(inNode),
              label(String::sEmpty())
        {
            load(
                "Assets/Engine/UI/Components/Window/Header/Index.grid",
                "Assets/Engine/UI/Components/Window/Header/Index.decal"
            );

            watchAttribute(LABEL_ATTRIBUTE_NAME, [this](const String& inValue) { label = parseText(inValue).trim(); });
        }

        void WindowHeader::onClose()
        {
            if (Window* window = Window::sFindFrom(this))
            {
                window->dismiss();
            }
        }
    }
}
