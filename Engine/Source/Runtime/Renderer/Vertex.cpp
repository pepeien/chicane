#include "Vertex.hpp"

namespace Engine
{
    namespace Vertex
    {
        vk::VertexInputBindingDescription getBindingDescription()
        {
            vk::VertexInputBindingDescription bindingDescription{};
            bindingDescription.binding   = 0;
            bindingDescription.stride    = sizeof(Instance);
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
            positionDescription.format   = vk::Format::eR32G32Sfloat;
            positionDescription.offset   = offsetof(Instance, position);

            attributeDescriptions.push_back(positionDescription);

            // Color
            vk::VertexInputAttributeDescription colorDescription;
            colorDescription.binding  = 0;
            colorDescription.location = 1;
            colorDescription.format   = vk::Format::eR32G32B32Sfloat;
            colorDescription.offset   = offsetof(Instance, color);

            attributeDescriptions.push_back(colorDescription);

            // Texture Position
            vk::VertexInputAttributeDescription texturePositionDescription;
            texturePositionDescription.binding  = 0;
            texturePositionDescription.location = 2;
            texturePositionDescription.format   = vk::Format::eR32G32Sfloat;
            texturePositionDescription.offset   = offsetof(Instance, texturePosition);

            attributeDescriptions.push_back(texturePositionDescription);

            return attributeDescriptions;
        }
    }
}