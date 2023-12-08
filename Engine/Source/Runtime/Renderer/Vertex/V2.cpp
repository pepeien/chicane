#include "V2.hpp"

namespace Engine
{
    namespace Runtime
    {
        namespace Renderer
        {
            namespace Vertex
            {
                vk::VertexInputBindingDescription V2::getBindingDescription()
                {
                    vk::VertexInputBindingDescription bindingDescription{};
                    bindingDescription.binding   = 0;
                    bindingDescription.stride    = sizeof(V2);
                    bindingDescription.inputRate = vk::VertexInputRate::eVertex;

                    return bindingDescription;
                }

                std::array<vk::VertexInputAttributeDescription, 2> V2::getAttributeDescriptions()
                {
                    std::array<vk::VertexInputAttributeDescription, 2> attributeDescriptions{};

                    // Position
                    attributeDescriptions[0].binding  = 0;
                    attributeDescriptions[0].location = 0;
                    attributeDescriptions[0].format   = vk::Format::eR32G32Sfloat;
                    attributeDescriptions[0].offset   = offsetof(V2, position);

                    // Color
                    attributeDescriptions[1].binding  = 0;
                    attributeDescriptions[1].location = 1;
                    attributeDescriptions[1].format   = vk::Format::eR32G32B32Sfloat;
                    attributeDescriptions[1].offset   = offsetof(V2, color);

                    return attributeDescriptions;
                }
			}
		}
	}
}