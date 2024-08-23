#pragma once

#include "Chicane/Base.hpp"
#include "Chicane/Core/Math.hpp"

namespace Chicane
{
    namespace Vertex
    {
        struct Instance
        {
            Math<float, 3>::Vec position;
            Math<float, 3>::Vec color;
            Math<float, 2>::Vec texturePosition;
            Math<float, 3>::Vec normal;
        };

        vk::VertexInputBindingDescription getBindingDescription();
        std::vector<vk::VertexInputAttributeDescription> getAttributeDescriptions();
    }
}