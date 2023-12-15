#pragma once

#include "Base.hpp"

namespace Engine
{
    namespace Runtime
    {
        namespace Renderer
        {
            namespace Surface
            {
                void init(
                    vk::SurfaceKHR& outSurface,
                    const vk::Instance& inInstance,
                    GLFWwindow* inWindow
                );
            }
        }
    }
}