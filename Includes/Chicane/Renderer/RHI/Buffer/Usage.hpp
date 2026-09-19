#pragma once

#include <cstdint>

#include "Chicane/Core/String.hpp"

namespace Chicane
{
    namespace Renderer
    {
        namespace RHI
        {
            enum class BufferUsage : std::uint8_t
            {
                Vertex,
                Index,
                Uniform,
                Storage
            };
        }
    }

    inline String toString(Renderer::RHI::BufferUsage inValue)
    {
        switch (inValue)
        {
        case Renderer::RHI::BufferUsage::Vertex:
            return "Vertex";

        case Renderer::RHI::BufferUsage::Index:
            return "Index";

        case Renderer::RHI::BufferUsage::Uniform:
            return "Uniform";

        case Renderer::RHI::BufferUsage::Storage:
            return "Storage";

        default:
            return "";
        }
    }
}
