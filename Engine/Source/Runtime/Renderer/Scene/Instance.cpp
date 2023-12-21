#include "Instance.hpp"

namespace Chicane
{
    namespace Scene
    {
        std::vector<Object::Instance> Instance::getObjects() const
        {
            return objects;
        }
    
        void Instance::addObject(const Object::Instance& inObject)
        {
            objects.push_back(inObject);
        }
    }
}