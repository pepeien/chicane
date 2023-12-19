#pragma once

#include "Base.hpp"

#include "Instance.hpp"

namespace Engine
{
    class Vertex2D : public Vertex::Base
    {
    public:
        static vk::VertexInputBindingDescription getBindingDescription();
        static std::vector<vk::VertexInputAttributeDescription> getAttributeDescriptions();
        static std::vector<Vertex::Base*> toBaseList(const std::vector<Vertex2D*>& inVertices);
    }; 
}