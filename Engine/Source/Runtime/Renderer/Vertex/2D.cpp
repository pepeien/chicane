#include "2D.hpp"

namespace Engine
{
    namespace Runtime
    {
        namespace Renderer
        {
            vk::VertexInputBindingDescription Vertex2D::getBindingDescription()
            {
                vk::VertexInputBindingDescription bindingDescription{};
                bindingDescription.binding   = 0;
                bindingDescription.stride    = sizeof(Vertex2D);
                bindingDescription.inputRate = vk::VertexInputRate::eVertex;

                return bindingDescription;
            }

            std::array<vk::VertexInputAttributeDescription, 2> Vertex2D::getAttributeDescriptions()
            {
                std::array<vk::VertexInputAttributeDescription, 2> attributeDescriptions{};

                // Position
                attributeDescriptions[0].binding  = 0;
                attributeDescriptions[0].location = 0;
                attributeDescriptions[0].format   = vk::Format::eR32G32Sfloat;
                attributeDescriptions[0].offset   = offsetof(Vertex2D, position);

                // Color
                attributeDescriptions[1].binding  = 0;
                attributeDescriptions[1].location = 1;
                attributeDescriptions[1].format   = vk::Format::eR32G32B32Sfloat;
                attributeDescriptions[1].offset   = offsetof(Vertex2D, color);

                return attributeDescriptions;
            }

            std::vector<Vertex::Base*> Vertex2D::toBaseList(std::vector<Vertex2D*>& inVertices)
            {
                std::vector<Vertex::Base*> result;

                for (Vertex2D* vertex : inVertices)
                {
                    result.push_back(vertex);
                }

                return result;
            }
        }
    }
}