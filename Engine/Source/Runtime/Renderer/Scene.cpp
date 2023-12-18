#include "Scene.hpp"

namespace Engine
{
    namespace Scene
    {
        std::vector<Object::Instance<Vertex::Base>> Instance::getObjects() const
        {
            return objects;
        }
    
        void Instance::addObject(const Object::Instance<Vertex2D>& inObject)
        {
            Object::Instance<Vertex::Base> convertedSceneObject;
            convertedSceneObject.vertices              = Vertex2D::toBaseList(inObject.vertices);
            convertedSceneObject.transform.translation = inObject.transform.translation;
            convertedSceneObject.transform.rotation    = inObject.transform.rotation;
            convertedSceneObject.transform.scale       = inObject.transform.scale;
    
            objects.push_back(convertedSceneObject);
        }
    }
}