#include "Window.hpp"

namespace Engine
{
    namespace Runtime
    {
        namespace Core
        {
            namespace Window
            {
                GLFWwindow* init(int& outWidth, int& outHeight, const char* inWindowTitle)
                {
                    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
                    glfwWindowHint(GLFW_RESIZABLE,  GLFW_TRUE);

                    const GLFWvidmode* desktop = glfwGetVideoMode(glfwGetPrimaryMonitor());

                    outWidth  = desktop->width;
                     outHeight = desktop->height;

                    return glfwCreateWindow(
                        desktop->width,
                        desktop->height,
                        inWindowTitle,
                        nullptr,
                        nullptr
                    );
                }
            }
        }
    }
}