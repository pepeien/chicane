#pragma once

#include "Chicane/Base.hpp"
#include "Chicane/Core/Math.hpp"

namespace Chicane
{
    namespace Vertex
    {
        struct Instance
        {
            Vec<3, float> position;
            Vec<3, float> color;
            Vec<2, float> texturePosition;
            Vec<3, float> normal;
        };

        vk::VertexInputBindingDescription getBindingDescription();
        std::vector<vk::VertexInputAttributeDescription> getAttributeDescriptions();
    }
}