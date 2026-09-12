#pragma once

#include <vector>

#include "Chicane/Core/Math/Vertex.hpp"
#include "Chicane/Core/Time.hpp"

#include "Chicane/Renderer.hpp"

namespace Chicane
{
    namespace Renderer
    {
        namespace Debug
        {

            struct CHICANE_RENDERER Trace
            {
            public:
                using List = std::vector<Trace>;

            public:
                static constexpr inline float DEFAULT_DURATION = 2.0f;

            public:
                Vertex::List vertices              = {};
                Time         expireAt              = {};
                bool         bIsPersistant         = false;
                bool         bInWillExpireOneFrame = false;
            };
        }
    }
}
