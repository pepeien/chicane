#pragma once

#include <cstdint>

#include "Chicane/Core/String.hpp"

namespace Chicane
{
    namespace Renderer
    {
        namespace RHI
        {
            enum class StoreOp : std::uint8_t
            {
                Store,
                DontCare
            };
        }
    }

    inline String toString(Renderer::RHI::StoreOp inValue)
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
