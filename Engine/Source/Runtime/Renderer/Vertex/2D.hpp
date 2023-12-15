#pragma once

#include "Base.hpp"

#include "Renderer/Vertex.hpp"

namespace Engine
{
    namespace Runtime
    {
        namespace Renderer
        {
            class Vertex2D : public Vertex::Base
            {
            public:
                static vk::VertexInputBindingDescription getBindingDescription();
                static std::array<vk::VertexInputAttributeDescription, 2> getAttributeDescriptions();
                static std::vector<Vertex::Base*> toBaseList(const std::vector<Vertex2D*>& inVertices);
            }; 
        }
    }
}