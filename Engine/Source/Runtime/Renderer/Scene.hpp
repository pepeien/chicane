#pragma once

#include "Base.hpp"

#include "Vertex/2D.hpp"

namespace Engine
{
    namespace Runtime
    {
        namespace Renderer
        {
            template<typename T>
            struct SceneObject
            {
                std::vector<T*> vertices;
                glm::vec3 position;
                glm::vec3 rotation;
                glm::vec3 transform;
            };

            class Scene
            {
            public:
                std::vector<SceneObject<Vertex::Base>> getObjects();
                void addObject(SceneObject<Vertex2D>& inObject);

            private:
                std::vector<SceneObject<Vertex::Base>> objects;
            };
        }
    }
}