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

        inline String toString(Resource inValue)
        {
            switch (inValue)
            {
            case Resource::Scene:
                return "Scene";

            case Resource::SceneIndices:
                return "SceneIndices";

            case Resource::SceneVertices:
                return "SceneVertices";

            case Resource::SceneInstances:
                return "SceneInstances";

            case Resource::SceneCamera:
                return "SceneCamera";

            case Resource::SceneLights:
                return "SceneLights";

            case Resource::Texture:
                return "Texture";

            case Resource::UI:
                return "UI";

            case Resource::UIIndices:
                return "UIIndices";

            case Resource::UIVertices:
                return "UIVertices";

            case Resource::UIInstances:
                return "UIInstances";

            case Resource::UIGlyphs:
                return "UIGlyphs";

            default:
                return "";
            }
        }

        using ResourceBudget = std::unordered_map<Resource, float>;
    }
}