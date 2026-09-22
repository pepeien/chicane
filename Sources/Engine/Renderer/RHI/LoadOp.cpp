#include "Chicane/Renderer/RHI/LoadOp.hpp"

namespace Chicane
{
    String toString(Renderer::RHI::LoadOp inValue)
    {
        switch (inValue)
        {
        case Renderer::RHI::LoadOp::Load:
            return "Load";

        case Renderer::RHI::LoadOp::Clear:
            return "Clear";

        case Renderer::RHI::LoadOp::DontCare:
            return "DontCare";

        default:
            return "";
        }
    }
}
