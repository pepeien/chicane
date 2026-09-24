#include "Chicane/Renderer/Feature.hpp"

namespace Chicane
{
    String toString(Renderer::RendererFeature inValue)
    {
        switch (inValue)
        {
        case Renderer::RendererFeature::None:
            return "None";

        case Renderer::RendererFeature::Fill:
            return "Fill";

        case Renderer::RendererFeature::Wireframe:
            return "Wireframe";

        case Renderer::RendererFeature::Bounds:
            return "Bounds";

        case Renderer::RendererFeature::Traces:
            return "Traces";

        case Renderer::RendererFeature::Colliders:
            return "Colliders";

        case Renderer::RendererFeature::Skeletons:
            return "Skeletons";

        case Renderer::RendererFeature::HDR:
            return "HDR";

        case Renderer::RendererFeature::Light:
            return "Light";

        case Renderer::RendererFeature::Outline:
            return "Outline";

        case Renderer::RendererFeature::Bloom:
            return "Bloom";

        case Renderer::RendererFeature::Default:
            return "Default";

        case Renderer::RendererFeature::All:
            return "All";

        default:
            return "";
        }
    }
}
