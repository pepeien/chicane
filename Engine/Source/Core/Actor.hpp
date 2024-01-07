#pragma once

#include "Base.hpp"

class Controller;

namespace Chicane
{
    struct Transform
    {
        glm::vec3 translation;
        glm::vec3 rotation;
        glm::vec3 scale;
    };

    class Actor
    {
    private:
        Transform m_transform;

        std::shared_ptr<Controller> m_controller;
    };
}