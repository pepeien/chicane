#pragma once

#include "Base.h"

namespace Engine
{
	namespace Core
	{
		class Mounter
		{
		public:
			// Vulkan
			void initVulkanDebugMessenger(
				vk::DebugUtilsMessengerEXT& inDebugMessenger,
				vk::Instance& inInstance,
				vk::DispatchLoaderDynamic& inDldi
			);

			void initVulkanInstance(vk::Instance& inInstance, vk::DispatchLoaderDynamic& inDldi);

			// GLFW
			GLFWwindow* initGlfwWindow();

		private:
			// Vulkan
			bool areExtensionsSupported(std::vector<const char*> inExtensions);
			bool areValidationLayersSupported(std::vector<const char*> inValidationLayers);
		};
	}
}