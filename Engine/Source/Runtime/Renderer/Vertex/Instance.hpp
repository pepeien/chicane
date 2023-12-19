#pragma once

#include "Base.hpp"

namespace Engine
{
    namespace Vertex
    {
		class Base
        {
        public:
            glm::vec2 position;
            glm::vec3 color;
            glm::vec2 texturePosition;
        };
	}
}