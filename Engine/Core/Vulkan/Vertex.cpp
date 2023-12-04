#include "Vertex.h"

namespace Engine
{
    namespace Core
    {
        namespace Vulkan
        {
            vk::VertexInputBindingDescription Vertex::getBindingDescription()
            {
                vk::VertexInputBindingDescription bindingDescription{};
                bindingDescription.binding   = 0;
                bindingDescription.stride    = sizeof(Vertex);
                bindingDescription.inputRate = vk::VertexInputRate::eInstance;

                return bindingDescription;
            }

            std::array<vk::VertexInputAttributeDescription, 2> Vertex::getAttributeDescriptions()
            {
                std::array<vk::VertexInputAttributeDescription, 2> attributeDescriptions{};

                //inPosition
                attributeDescriptions[0].binding  = 0;
                attributeDescriptions[0].location = 0;
                attributeDescriptions[0].format   = vk::Format::eR32G32Sfloat;
                attributeDescriptions[0].offset   = offsetof(Vertex, pos);

                // inColor
                attributeDescriptions[1].binding  = 0;
                attributeDescriptions[1].location = 1;
                attributeDescriptions[1].format   = vk::Format::eR32G32B32Sfloat;
                attributeDescriptions[1].offset   = offsetof(Vertex, color);

                return attributeDescriptions;
            }
        }
    }
}