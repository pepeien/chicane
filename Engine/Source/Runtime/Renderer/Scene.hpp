#pragma once

#include "Base.hpp"

#include "Scene/Object.hpp"
#include "Vertex/2D.hpp"

namespace Engine
{
    namespace Runtime
    {
        namespace Renderer
        {
            namespace Scene
            {
                class Instance
                {
                public:
                    std::vector<Object<Vertex::Base>> getObjects();
                    void addObject(Object<Vertex2D>& inObject);

                private:
                    std::vector<Object<Vertex::Base>> objects;
                };
            }
        }
    }
}