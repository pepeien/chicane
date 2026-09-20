#pragma once

#include "Chicane/Renderer.hpp"

namespace Chicane
{
    namespace Renderer
    {
        CHICANE_RENDERER void OpenGLDebugCallback(
            unsigned int source,
            unsigned int type,
            unsigned int id,
            unsigned int severity,
            int          length,
            const char*  message,
            const void*  userParam
        );
    }
}