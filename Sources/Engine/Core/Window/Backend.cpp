#include "Chicane/Core/Window/Backend.hpp"

namespace Chicane
{
    String toString(WindowBackend inValue)
    {
        switch (inValue)
        {
        case WindowBackend::Undefined:
            return "Undefined";

        case WindowBackend::OpenGL:
            return "OpenGL";

        case WindowBackend::Vulkan:
            return "Vulkan";

        default:
            return "";
        }
    }
}
