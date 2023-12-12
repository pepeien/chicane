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
                    convertedSceneObject.translation = inObject.translation;
                    convertedSceneObject.rotation    = inObject.rotation;
                    convertedSceneObject.scale       = inObject.scale;
                    convertedSceneObject.vertices    = Vertex2D::toBaseList(inObject.vertices);

                    objects.push_back(convertedSceneObject);
                }
            }
        }
    }
}