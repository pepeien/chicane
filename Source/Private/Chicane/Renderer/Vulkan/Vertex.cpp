#include "Chicane/Renderer/Vulkan/Vertex.hpp"

#include "Chicane/Box/Asset/Model/Data.hpp"

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
                bindingDescription.stride    = sizeof(Box::Model::Vertex);
                bindingDescription.inputRate = vk::VertexInputRate::eVertex;

                return bindingDescription;
            }

            std::vector<vk::VertexInputAttributeDescription> getAttributeDescriptions()
            {
                std::vector<vk::VertexInputAttributeDescription> attributeDescriptions;

                // Position
                vk::VertexInputAttributeDescription positionDescription;
                positionDescription.binding  = 0;
                positionDescription.location = 0;
                positionDescription.format   = vk::Format::eR32G32B32Sfloat;
                positionDescription.offset   = offsetof(Box::Model::Vertex, position);

                attributeDescriptions.push_back(positionDescription);

                // Color
                vk::VertexInputAttributeDescription colorDescription;
                colorDescription.binding  = 0;
                colorDescription.location = 1;
                colorDescription.format   = vk::Format::eR32G32B32Sfloat;
                colorDescription.offset   = offsetof(Box::Model::Vertex, color);

                attributeDescriptions.push_back(colorDescription);

                // UV
                vk::VertexInputAttributeDescription UVDescription;
                UVDescription.binding  = 0;
                UVDescription.location = 2;
                UVDescription.format   = vk::Format::eR32G32Sfloat;
                UVDescription.offset   = offsetof(Box::Model::Vertex, UV);

                attributeDescriptions.push_back(UVDescription);

                // Normals
                vk::VertexInputAttributeDescription normalsDescription;
                normalsDescription.binding  = 0;
                normalsDescription.location = 3;
                normalsDescription.format   = vk::Format::eR32G32B32Sfloat;
                normalsDescription.offset   = offsetof(Box::Model::Vertex, normal);

                attributeDescriptions.push_back(normalsDescription);

                return attributeDescriptions;
            }
        }
    }
}