#pragma once

#include <cstdint>
#include <unordered_map>

#include "Chicane/Core/String.hpp"

namespace Chicane
{
    namespace Renderer
    {
        enum class Resource : std::uint8_t
        {
            Scene,
            SceneIndices,
            SceneVertices,
            SceneInstances,
            SceneCamera,
            SceneLights,

            Texture,

            UI,
            UIIndices,
            UIVertices,
            UIInstances,
            UIGlyphs
        };

        using ResourceBudget = std::unordered_map<Resource, float>;
    }

    inline String toString(Renderer::Resource inValue)
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
