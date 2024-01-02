#pragma once

#include "Base.hpp"

#include "Instance.hpp"

namespace Chicane
{
    namespace Level
    {
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

            struct Pawn : public Actor::Instance
            {
                Mesh mesh;
                Texture texture;
            };
        }
    }
}