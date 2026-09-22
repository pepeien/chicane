#include "Chicane/Renderer/RHI/Buffer/Usage.hpp"

namespace Chicane
{
    String toString(Renderer::RHI::BufferUsage inValue)
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
