#pragma once

#include <cstdint>
#include <unordered_map>

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

            Texture,

            UI,
            UIIndices,
            UIVertices,
            UIInstances,

            Misc
        };

        using ResourceBudget = std::unordered_map<Resource, float>;
    }
}