#pragma once

#include "Base.hpp"
#include "Buffer.hpp"

namespace Chicane
{
    namespace Model
    {
        struct Bundle
        {
            std::vector<glm::mat4> transforms;
            size_t allocationSize;
            void* writeLocation;
            Buffer::Instance buffer;
        };
    }
}