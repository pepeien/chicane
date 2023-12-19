#pragma once

#include "Base.hpp"

#include "Object.hpp"

namespace Engine
{
    namespace Scene
    {
        class Instance
        {
        public:
            std::vector<Object::Instance> getObjects() const;
            void addObject(const Object::Instance& inObject);

        public:
            std::vector<Object::Instance> objects;
        };
    }
}