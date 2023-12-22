#pragma once

#include "Base.hpp"

namespace Chicane
{
    namespace Window
    {
        struct Instance
        {
            GLFWwindow* instance;
            std::string title;
            int width;
            int height;
        };

        void init(Instance& outWindow);
    }
}