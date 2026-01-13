#include "Chicane/Runtime/Renderer/OpenGL/Debug.hpp"

#include "Chicane/Core/Color.hpp"
#include "Chicane/Core/Log.hpp"
#include "Chicane/Core/String.hpp"

namespace Chicane
{
    namespace OpenGL
    {
        namespace Debug
        {
            void debugCallback(
                GLenum        source,
                GLenum        type,
                GLuint        id,
                GLenum        severity,
                GLsizei       length,
                const GLchar* message,
                const void*   userParam
            )
            {
                if (!message)
                {
                    return;
                }

                String prefix = "General";

                switch (type)
                {
                case GL_DEBUG_TYPE_PERFORMANCE:
                    prefix = "Performance";
                    break;

                case GL_DEBUG_TYPE_ERROR:
                case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
                case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
                    prefix = "Validation";
                    break;

                case GL_DEBUG_TYPE_PORTABILITY:
                    prefix = "Portability";
                    break;

                case GL_DEBUG_TYPE_MARKER:
                case GL_DEBUG_TYPE_PUSH_GROUP:
                case GL_DEBUG_TYPE_POP_GROUP:
                    prefix = "Marker";
                    break;
                }

                String color = Color::HEX_COLOR_WHITE;

                switch (severity)
                {
                case GL_DEBUG_SEVERITY_HIGH:
                    color = Color::HEX_COLOR_ORANGE;
                    break;

                case GL_DEBUG_SEVERITY_MEDIUM:
                    color = Color::HEX_COLOR_YELLOW;
                    break;

                case GL_DEBUG_SEVERITY_LOW:
                    color = Color::HEX_COLOR_CYAN;
                    break;

                case GL_DEBUG_SEVERITY_NOTIFICATION:
                    return;
                }

                Log::emmit(color, "OpenGL", prefix + " | " + message);
            }
        }
    }
}