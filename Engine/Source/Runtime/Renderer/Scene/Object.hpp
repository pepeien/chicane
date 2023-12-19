#pragma once

#include "Base.hpp"

#include "Renderer/Texture/Instance.hpp"

namespace Engine
{
    namespace Scene
    {
        namespace Object
		{
			struct Transform
			{
				glm::vec3 translation;
			    glm::vec3 rotation;
			    glm::vec3 scale;
			};

			struct Instance
			{
			    std::string mesh;
				std::string texture;
			    Transform transform;
			};
		}
	}
}