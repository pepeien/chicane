#include "Chicane/Renderer/Shader/Type.hpp"
#include "Chicane/Renderer/Shader/Type.reflected.hpp"

namespace Chicane
{
    String toString(Renderer::ShaderType inValue)
    {
        switch (inValue)
        {
        case Renderer::ShaderType::Fragment:
            return "Fragment";

        case Renderer::ShaderType::Vertex:
            return "Vertex";

        case Renderer::ShaderType::Undefined:
            return "Undefined";

        default:
            return "";
        }
    }
}
