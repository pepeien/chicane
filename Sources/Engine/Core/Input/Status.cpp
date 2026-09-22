#include "Chicane/Core/Input/Status.hpp"

namespace Chicane
{
    String toString(Input::Status inValue)
    {
        switch (inValue)
        {
        case Input::Status::Pressed:
            return "Pressed";

        case Input::Status::Released:
            return "Released";

        default:
            return "";
        }
    }
}
