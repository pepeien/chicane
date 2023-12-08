#pragma once

#include "Base.hpp"

namespace Engine
{
    namespace Runtime
    {
        namespace Renderer
        {
            class Scene
            {
            public:
                Scene();

            public:
                std::vector<glm::vec3> positions;
            };
        }
    }
}