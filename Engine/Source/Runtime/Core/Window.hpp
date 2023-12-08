#pragma once

#include "Base.hpp"

namespace Engine
{
    namespace Runtime
    {
        namespace Core
        {
            namespace Window
            {
                GLFWwindow* init(int& outWidth, int& outHeight, const char* inWindowTitle);
            }
        }
    }
}