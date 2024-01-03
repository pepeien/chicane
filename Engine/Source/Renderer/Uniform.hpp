#pragma once

#include "Base.hpp"

namespace Chicane
{
    namespace Uniform
    {
    	struct BufferObject
        {
            glm::mat4 view;
            glm::mat4 projection;
            glm::mat4 viewProjection;
        };
    }
}