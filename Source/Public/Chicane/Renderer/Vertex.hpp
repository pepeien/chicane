#pragma once

#include "Chicane/Base.hpp"
#include "Chicane/Core/Math.hpp"

namespace Chicane
{
    namespace Vertex
    {
        struct Instance
        {
            Vec<float>::Three position;
            Vec<float>::Three color;
            Vec<float>::Two texturePosition;
            Vec<float>::Three normal;
        };

        vk::VertexInputBindingDescription getBindingDescription();
        std::vector<vk::VertexInputAttributeDescription> getAttributeDescriptions();
    }
}