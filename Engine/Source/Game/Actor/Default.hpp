#pragma once

#include "Base.hpp"

namespace Engine
{
    namespace Actor
    {
        struct Transform
        {
            glm::vec3 translation = glm::vec3(1.0f);
            glm::vec3 rotation    = glm::vec3(0.0f);
            glm::vec3 scale       = glm::vec3(1.0f);
        };

        struct Mesh
        {
            std::string model;
            std::string texture;
        };

        class Default
        {
        public:
            Default();

        public:
            glm::mat4 getPosition();

            void translate(const glm::vec3& inTranslation);
            void rotate(const glm::vec3& inRotation);
            void scale(const glm::vec3& inScale);
        
            void setMesh(const Mesh& inMesh);

        protected:
            glm::mat4 m_position;
            Transform m_transform;

            Mesh m_mesh;
        };
    }
}