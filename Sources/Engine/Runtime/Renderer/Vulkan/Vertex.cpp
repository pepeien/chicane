#include "Chicane/Runtime/Renderer/Vulkan/Vertex.hpp"

#include "Chicane/Core/Math/Vertex.hpp"

namespace Chicane
{
    namespace Vulkan
    {
        namespace Vertex
        {
            vk::VertexInputBindingDescription getBindingDescription()
            {
                vk::VertexInputBindingDescription bindingDescription{};
                bindingDescription.binding   = 0;
                bindingDescription.stride    = sizeof(Chicane::Vertex);
                bindingDescription.inputRate = vk::VertexInputRate::eVertex;

                return bindingDescription;
            }

            std::vector<vk::VertexInputAttributeDescription>
            getAttributeDescriptions()
            {
                std::vector<vk::VertexInputAttributeDescription>
                    attributeDescriptions;

                // Position
                vk::VertexInputAttributeDescription positionDescription;
                positionDescription.binding  = 0;
                positionDescription.location = 0;
                positionDescription.format   = vk::Format::eR32G32B32Sfloat;
                positionDescription.offset =
                    offsetof(Chicane::Vertex, position);

                attributeDescriptions.push_back(positionDescription);

                // Color
                vk::VertexInputAttributeDescription colorDescription;
                colorDescription.binding  = 0;
                colorDescription.location = 1;
                colorDescription.format   = vk::Format::eR32G32B32A32Sfloat;
                colorDescription.offset   = offsetof(Chicane::Vertex, color);

                attributeDescriptions.push_back(colorDescription);

                // UV
                vk::VertexInputAttributeDescription UVDescription;
                UVDescription.binding  = 0;
                UVDescription.location = 2;
                UVDescription.format   = vk::Format::eR32G32Sfloat;
                UVDescription.offset   = offsetof(Chicane::Vertex, uv);

                attributeDescriptions.push_back(UVDescription);

                // Normal
                vk::VertexInputAttributeDescription normalsDescription;
                normalsDescription.binding  = 0;
                normalsDescription.location = 3;
                normalsDescription.format   = vk::Format::eR32G32B32Sfloat;
                normalsDescription.offset   = offsetof(Chicane::Vertex, normal);

                attributeDescriptions.push_back(normalsDescription);

                return attributeDescriptions;
            }
        }
    }
}