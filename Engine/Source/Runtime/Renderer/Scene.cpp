#include "Scene.hpp"

namespace Engine
{
    namespace Runtime
    {
        namespace Renderer
        {
            std::vector<SceneObject<Vertex::Base>> Scene::getObjects()
            {
                return objects;
            }

            void Scene::addObject(SceneObject<Vertex2D>& inObject)
            {
                SceneObject<Vertex::Base> convertedSceneObject;
                convertedSceneObject.position = inObject.position;
                convertedSceneObject.vertices = Vertex2D::toBaseList(inObject.vertices);

                objects.push_back(convertedSceneObject);
            }
        }
    }
}