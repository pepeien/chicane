#pragma once

#include "Base.hpp"

#include "Vertex.hpp"
#include "Texture.hpp"

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
	
			template<typename T>
			struct Instance
			{
			    std::vector<T*> vertices;
			    Transform transform;
				Texture::Instance* texture;
			};
		}

        class Instance
        {
        public:
            std::vector<Object::Instance<Vertex::Base>> getObjects() const;
            void addObject(const Object::Instance<Vertex2D>& inObject);

        public:
            std::vector<Object::Instance<Vertex::Base>> objects;
        };
    }
}