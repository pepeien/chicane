#pragma once

#include "Base.hpp"

namespace Engine
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