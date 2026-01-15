#pragma once

#include <GL/glew.h>

#include "Chicane/Runtime.hpp"

namespace Chicane
{
    namespace OpenGL
    {
        namespace Debug
        {
            CHICANE_RUNTIME void debugCallback(
                GLenum        source,
                GLenum        type,
                GLuint        id,
                GLenum        severity,
                GLsizei       length,
                const GLchar* message,
                const void*   userParam
            );
        }
    }
}