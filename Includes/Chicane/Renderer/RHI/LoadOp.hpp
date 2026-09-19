#pragma once

#include <cstdint>

#include "Chicane/Core/String.hpp"

namespace Chicane
{
    namespace Renderer
    {
        namespace RHI
        {
            enum class LoadOp : std::uint8_t
            {
                Load,
                Clear,
                DontCare
            };
        }
    }

    inline String toString(Renderer::RHI::LoadOp inValue)
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
