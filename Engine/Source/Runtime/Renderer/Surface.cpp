#include "Surface.hpp"

namespace Engine
{
	namespace Runtime
	{
		namespace Renderer
		{
			namespace Surface
			{
				void init(vk::SurfaceKHR& outSurface, vk::Instance& inInstance, GLFWwindow* inWindow)
				{
					VkSurfaceKHR c_style_surface;

            		if (glfwCreateWindowSurface(inInstance, inWindow, nullptr, &c_style_surface) != VK_SUCCESS)
            		{
            		    throw std::runtime_error("Failed to abtract the GLFW surface for Vulkan");
            		}

					outSurface = c_style_surface;
				}
			}
		}
	}
}