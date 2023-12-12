#pragma once

#include "Base.hpp"

namespace Engine
{
    namespace Runtime
    {
        namespace Renderer
        {
			namespace Scene
			{
				template<typename T>
           		struct Object
           		{
           		    std::vector<T*> vertices;
           		    glm::vec3 translation;
           		    glm::vec3 rotation;
           		    glm::vec3 scale;
           		};
			}
		}
	}
}