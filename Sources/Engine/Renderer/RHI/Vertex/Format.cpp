#include "Chicane/Renderer/RHI/Vertex/Format.hpp"

namespace Chicane
{
    String toString(Renderer::RHI::VertexFormat inValue)
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
