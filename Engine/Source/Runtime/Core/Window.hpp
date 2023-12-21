#pragma once

#include "Base.hpp"

namespace Chicane
{
    namespace Window
    {
        GLFWwindow* init(int& outWidth, int& outHeight, const char* inWindowTitle);
    }
}