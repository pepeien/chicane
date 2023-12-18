#pragma once

#include "Base.hpp"

namespace Engine
{
    namespace Window
    {
        GLFWwindow* init(int& outWidth, int& outHeight, const char* inWindowTitle);
    }
}