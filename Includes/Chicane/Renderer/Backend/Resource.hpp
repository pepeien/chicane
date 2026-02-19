#pragma once

#include <cstdint>

namespace Chicane
{
    namespace Renderer
    {
        enum class BackendResource : std::uint8_t
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
    }
}