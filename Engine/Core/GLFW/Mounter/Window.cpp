#include "Window.h"

namespace Engine
{
	namespace Core
	{
		namespace GLFW
		{
			namespace Mounter
			{
				GLFWwindow* initWindow(int& widthAllocator, int& heightAllocator)
				{
					glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
            		glfwWindowHint(GLFW_RESIZABLE,  GLFW_TRUE);

            		const GLFWvidmode* desktop = glfwGetVideoMode(glfwGetPrimaryMonitor());

					widthAllocator  = desktop->width;
            	 	heightAllocator = desktop->height;

            		return glfwCreateWindow(
            		    desktop->width,
            		    desktop->height,
            		    APPLICATION_NAME,
            		    nullptr,
            		    nullptr
            		);
				}
			}
		}
	}
}