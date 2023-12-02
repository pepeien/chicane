#pragma once

#include "Base.h"

namespace Engine
{
	namespace Core
	{
		namespace Mounter
		{
			namespace Vulkan
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