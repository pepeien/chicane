#include "Chicane/Renderer/RHI/StoreOp.hpp"

namespace Chicane
{
    String toString(Renderer::RHI::StoreOp inValue)
    {
        switch (inValue)
        {
        case Renderer::RHI::StoreOp::Store:
            return "Store";

        case Renderer::RHI::StoreOp::DontCare:
            return "DontCare";

        default:
            return "";
        }
    }
}
