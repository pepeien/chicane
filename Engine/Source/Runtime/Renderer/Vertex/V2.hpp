#pragma once

#include "Base.hpp"

#include "Renderer/Vertex.hpp"

namespace Engine
{
    namespace Runtime
    {
        namespace Renderer
        {
            namespace Vertex
            {
                struct V2 : Base
                {
                public:
                    static vk::VertexInputBindingDescription getBindingDescription();
                    static std::array<vk::VertexInputAttributeDescription, 2> getAttributeDescriptions();
                };  
            }
        }
    }
}