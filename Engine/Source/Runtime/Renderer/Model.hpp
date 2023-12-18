#pragma once

#include "Base.hpp"

namespace Engine
{
    namespace Model
    {
    	struct BufferObject
        {
            std::vector<glm::mat4> transforms;
        };
    }
}