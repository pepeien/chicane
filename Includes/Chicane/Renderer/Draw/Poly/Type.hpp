#pragma once

#include <cstdint>

#include "Chicane/Core/String.hpp"

namespace Chicane
{
    namespace Renderer
    {
        enum class DrawPolyType : std::uint8_t
        {
            e2D,
            e3D,
            eParticle
        };
    }

    inline String toString(Renderer::DrawPolyType inValue)
    {
        switch (inValue)
        {
        case Renderer::DrawPolyType::e2D:
            return "e2D";

        case Renderer::DrawPolyType::e3D:
            return "e3D";

        case Renderer::DrawPolyType::eParticle:
            return "eParticle";

        default:
            return "";
        }
    }
}
