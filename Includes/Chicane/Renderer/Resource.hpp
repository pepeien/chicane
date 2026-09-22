#pragma once

#include <cstdint>
#include <unordered_map>

#include "Chicane/Core/String.hpp"
#include "Chicane/Renderer.hpp"

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

    CHICANE_RENDERER String toString(Renderer::Resource inValue);
}
