#include "V3.hpp"

namespace Engine
{
    namespace Runtime
    {
        namespace Renderer
        {
            namespace Vertex
            {
                vk::VertexInputBindingDescription V3::getBindingDescription()
                {
                    vk::VertexInputBindingDescription bindingDescription{};
                    bindingDescription.binding   = 0;
                    bindingDescription.stride    = sizeof(V3);
                    bindingDescription.inputRate = vk::VertexInputRate::eVertex;

                    return bindingDescription;
                }

                std::array<vk::VertexInputAttributeDescription, 2> V3::getAttributeDescriptions()
                {
                    std::array<vk::VertexInputAttributeDescription, 2> attributeDescriptions{};

                    // Position
                    attributeDescriptions[0].binding  = 0;
                    attributeDescriptions[0].location = 0;
                    attributeDescriptions[0].format   = vk::Format::eR32G32Sfloat;
                    attributeDescriptions[0].offset   = offsetof(V3, position);

                    // Color
                    attributeDescriptions[1].binding  = 0;
                    attributeDescriptions[1].location = 1;
                    attributeDescriptions[1].format   = vk::Format::eR32G32B32Sfloat;
                    attributeDescriptions[1].offset   = offsetof(V3, color);
                    
                    // Texture Coordinates
                    attributeDescriptions[2].binding  = 0;
                    attributeDescriptions[2].location = 2;
                    attributeDescriptions[2].format   = vk::Format::eR32G32B32Sfloat;
                    attributeDescriptions[2].offset   = offsetof(V3, textureCoordinates);
                    
                    // Normals
                    attributeDescriptions[3].binding  = 0;
                    attributeDescriptions[3].location = 3;
                    attributeDescriptions[3].format   = vk::Format::eR32G32B32Sfloat;
                    attributeDescriptions[3].offset   = offsetof(V3, normals);

                    return attributeDescriptions;
                }
			}
		}
	}
}