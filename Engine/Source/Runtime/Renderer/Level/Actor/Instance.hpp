#pragma once

#include "Base.hpp"

namespace Chicane
{
    namespace Level
    {
        struct Transform
        {
            glm::vec3 translation;
            glm::vec3 rotation;
            glm::vec3 scale;
        };

        namespace Actor 
        {
            struct Instance
            {
                Transform transform;
            };
        }
    }
}