#pragma once

#include <GL/glew.h>

#include "Chicane/Runtime.hpp"

namespace Chicane
{
    namespace OpenGL
    {
        struct CHICANE_RUNTIME RendererInternals
        {
        public:
            GLuint meshBuffer;
            GLuint cameraBuffer;
            GLuint lightBuffer;
        };
    }
}