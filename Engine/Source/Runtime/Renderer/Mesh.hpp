#pragma once

#include "Base.hpp"

namespace Engine
{
	namespace Runtime
	{
		namespace Renderer
		{
			namespace Mesh
			{
				vk::VertexInputBindingDescription getPosColorBindingDescription();
				std::array<vk::VertexInputAttributeDescription, 2> getPosColorAttributeDescription();
			}
		}
	}
}