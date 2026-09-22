#include "Chicane/Renderer/Resource.hpp"

namespace Chicane
{
    String toString(Renderer::Resource inValue)
    {
        switch (inValue)
        {
        case Renderer::Resource::Scene:
            return "Scene";

        case Renderer::Resource::SceneIndices:
            return "SceneIndices";

        case Renderer::Resource::SceneVertices:
            return "SceneVertices";

        case Renderer::Resource::SceneInstances:
            return "SceneInstances";

        case Renderer::Resource::SceneCamera:
            return "SceneCamera";

        case Renderer::Resource::SceneLights:
            return "SceneLights";

        case Renderer::Resource::Texture:
            return "Texture";

        case Renderer::Resource::UI:
            return "UI";

        case Renderer::Resource::UIIndices:
            return "UIIndices";

        case Renderer::Resource::UIVertices:
            return "UIVertices";

        case Renderer::Resource::UIInstances:
            return "UIInstances";

        case Renderer::Resource::UIGlyphs:
            return "UIGlyphs";

        default:
            return "";
        }
    }
}
