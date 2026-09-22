#include "Chicane/Renderer/Backend/Status.hpp"

namespace Chicane
{
    String toString(Renderer::BackendStatus inValue)
    {
        switch (inValue)
        {
        case Renderer::BackendStatus::Running:
            return "Running";

        case Renderer::BackendStatus::Shutdown:
            return "Shutdown";

        default:
            return "";
        }
    }
}
