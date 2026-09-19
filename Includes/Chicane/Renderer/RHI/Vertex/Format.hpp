#pragma once

#include <cstdint>

#include "Chicane/Core/String.hpp"

namespace Chicane
{
    namespace Renderer
    {
        namespace RHI
        {
            enum class VertexFormat : std::uint8_t
            {
                Float2,
                Float3,
                Float4
            };
        }
    }

    inline String toString(Renderer::RHI::VertexFormat inValue)
    {
        switch (inValue)
        {
        case Renderer::RHI::VertexFormat::Float2:
            return "Float2";

        case Renderer::RHI::VertexFormat::Float3:
            return "Float3";

        case Renderer::RHI::VertexFormat::Float4:
            return "Float4";

        default:
            return "";
        }
    }
}
