#include "Window.hpp"

namespace Chicane
{
    namespace Window
    {
        void init(Instance& outWindow)
        {
            glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
            glfwWindowHint(GLFW_RESIZABLE,  GLFW_TRUE);
            glfwWindowHint(GLFW_MAXIMIZED,  GLFW_TRUE);

            const GLFWvidmode* desktop = glfwGetVideoMode(glfwGetPrimaryMonitor());

            outWindow.width  = desktop->width;
            outWindow.height = desktop->height;

            outWindow.instance = glfwCreateWindow(
                desktop->width,
                desktop->height,
                outWindow.title.c_str(),
                nullptr,
                nullptr
            );
        }
    }
}