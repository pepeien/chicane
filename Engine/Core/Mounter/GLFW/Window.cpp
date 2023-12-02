#include "Window.h"

namespace Engine
{
	namespace Core
	{
		namespace Mounter
		{
			namespace GLFW
			{
				GLFWwindow* initWindow()
				{
					glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
            		glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

            		const GLFWvidmode* desktop = glfwGetVideoMode(glfwGetPrimaryMonitor());

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