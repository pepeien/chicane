#include "Scene.hpp"

namespace Engine
{
    namespace Runtime
    {
        namespace Renderer
        {
            namespace Scene
            {
                std::vector<Object<Vertex::Base>> Instance::getObjects()
                {
                    return objects;
                }

                void Instance::addObject(Object<Vertex2D>& inObject)
                {
                    Object<Vertex::Base> convertedSceneObject;
                    convertedSceneObject.vertices              = Vertex2D::toBaseList(inObject.vertices);
                    convertedSceneObject.transform.translation = inObject.transform.translation;
                    convertedSceneObject.transform.rotation    = inObject.transform.rotation;
                    convertedSceneObject.transform.scale       = inObject.transform.scale;

                    objects.push_back(convertedSceneObject);
                }
            }
        }
    }
}