#pragma once

#include "Base.hpp"

namespace Engine
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

    class Actor
    {
    public:
        Actor();

    public:
        glm::mat4 getPosition();

        void setRelativeTranslation(const glm::vec3& inTranslation);
        void setRelativeRotation(const glm::vec3& inRotation);
        void setRelativeScale(const glm::vec3& inScale);

        void setAbsoluteTranslation(const glm::vec3& inTranslation);
        void setAbsoluteRotation(const glm::vec3& inRotation);
        void setAbsoluteScale(const glm::vec3& inScale);
    
        void setMesh(const Mesh& inMesh);

    private:
        void setTranslation(const glm::mat4& inBase, const glm::vec3& inTranslation);
        void setRotation(const glm::mat4& inBase, const glm::vec3& inRotation);
        void setScale(const glm::mat4& inBase, const glm::vec3& inScale);

    protected:
        glm::mat4 m_position;
        Transform m_transform;

        Mesh m_mesh;
    };
}