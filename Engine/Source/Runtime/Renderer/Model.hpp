#pragma once

#include "Base.hpp"

namespace Engine
{
    namespace Runtime
    {
        namespace Renderer
        {
			namespace Model
			{
				struct BufferObject
                {
                    std::vector<glm::mat4> transforms;
                };
			}
		}
	}
}