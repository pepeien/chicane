#pragma once

#include "Base.h"

namespace Engine
{
	namespace Core
	{
		namespace Vulkan
		{
			namespace Mounter
			{
				void initDebugMessenger(
					vk::DebugUtilsMessengerEXT& allocator,
					vk::Instance& inInstance,
					vk::DispatchLoaderDynamic& inDldi
				);
			}
		}
	}
}