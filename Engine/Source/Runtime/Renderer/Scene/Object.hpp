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
				struct ObjectTransform
				{
					glm::vec3 translation;
           		    glm::vec3 rotation;
           		    glm::vec3 scale;
				};

				template<typename T>
           		struct Object
           		{
           		    std::vector<T*> vertices;
           		    ObjectTransform transform;
           		};
			}
		}
	}
}