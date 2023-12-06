#pragma once

#include "Base.hpp"

namespace Engine
{
	namespace Core
	{
		namespace Render
		{
			namespace Debug
			{
				void initMessenger(
					vk::DebugUtilsMessengerEXT& allocator,
					vk::Instance& inInstance,
					vk::DispatchLoaderDynamic& inDldi
				);
			}
		}
	}
}