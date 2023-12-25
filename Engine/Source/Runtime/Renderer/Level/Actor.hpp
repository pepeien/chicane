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
            struct Mesh
            {
                std::string id;
            };

            struct Texture
            {
                std::string id;
            };

            struct Instance
            {
                Mesh mesh;
                Texture texture;
                Transform transform;
            };
        }
    }
}