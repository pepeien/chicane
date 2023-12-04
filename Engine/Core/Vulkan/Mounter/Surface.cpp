#include "Surface.h"

namespace Engine
{
	namespace Core
	{
		namespace Vulkan
		{
			namespace Mounter
			{
				void initSurface(vk::SurfaceKHR& allocator, vk::Instance& inInstance, GLFWwindow* inWindow)
				{
					VkSurfaceKHR c_style_surface;

            		if (glfwCreateWindowSurface(inInstance, inWindow, nullptr, &c_style_surface) != VK_SUCCESS)
            		{
            		    throw std::runtime_error("Failed to abtract the GLFW surface for Vulkan");
            		}

					allocator = c_style_surface;
				}
			}
		}
	}
}