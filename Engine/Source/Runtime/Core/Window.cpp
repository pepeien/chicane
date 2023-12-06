#include "Window.hpp"

namespace Engine
{
	namespace Core
	{
		namespace Window
		{
			GLFWwindow* init(int& widthAllocator, int& heightAllocator, const char* inWindowTitle)
			{
				glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
            	glfwWindowHint(GLFW_RESIZABLE,  GLFW_TRUE);

            	const GLFWvidmode* desktop = glfwGetVideoMode(glfwGetPrimaryMonitor());

				widthAllocator  = desktop->width;
             	heightAllocator = desktop->height;

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