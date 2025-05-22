#include "Chicane/Grid/Component/Button.hpp"

namespace Chicane
{
    namespace Grid
    {
        Button::Button(const pugi::xml_node& inNode)
            : Super(inNode)
        {
            addFunction(
                ON_CLICK_ATTRIBUTE_NAME,
                [this](const Event& inEvent)
                {
                    onClick(inEvent);

                    return Reference();
                }
            );
        }
    }
}