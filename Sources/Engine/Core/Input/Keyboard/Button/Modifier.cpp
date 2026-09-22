#include "Chicane/Core/Input/Keyboard/Button/Modifier.hpp"

namespace Chicane
{
    String toString(Input::KeyboardButtonModifier inValue)
    {
        switch (inValue)
        {
        case Input::KeyboardButtonModifier::None:
            return "None";

        case Input::KeyboardButtonModifier::LeftShift:
            return "LeftShift";

        case Input::KeyboardButtonModifier::RightShift:
            return "RightShift";

        case Input::KeyboardButtonModifier::Leve5:
            return "Leve5";

        case Input::KeyboardButtonModifier::LeftCtrl:
            return "LeftCtrl";

        case Input::KeyboardButtonModifier::RightCtrl:
            return "RightCtrl";

        case Input::KeyboardButtonModifier::LeftAlt:
            return "LeftAlt";

        case Input::KeyboardButtonModifier::RightAlt:
            return "RightAlt";

        case Input::KeyboardButtonModifier::LeftGui:
            return "LeftGui";

        case Input::KeyboardButtonModifier::RightGui:
            return "RightGui";

        case Input::KeyboardButtonModifier::Num:
            return "Num";

        case Input::KeyboardButtonModifier::Caps:
            return "Caps";

        case Input::KeyboardButtonModifier::Mode:
            return "Mode";

        case Input::KeyboardButtonModifier::Scroll:
            return "Scroll";

        case Input::KeyboardButtonModifier::Ctrl:
            return "Ctrl";

        case Input::KeyboardButtonModifier::Shift:
            return "Shift";

        case Input::KeyboardButtonModifier::Alt:
            return "Alt";

        case Input::KeyboardButtonModifier::Gui:
            return "Gui";

        default:
            return "";
        }
    }
}
