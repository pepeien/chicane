#pragma once

#include "Base.hpp"

namespace Chicane
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