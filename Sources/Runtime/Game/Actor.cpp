#include "Game/Actor.hpp"

#include "Game/Components/Actor.hpp"

namespace Chicane
{
    Actor::Actor()
        : m_position(glm::mat4(1.0f)),
        m_transform({})
    {}

    glm::mat4 Actor::getPosition()
    {
        return m_position;
    }

    void Actor::setRelativeTranslation(const glm::vec3& inTranslation)
    {
        m_transform.translation += inTranslation;

        setTranslation(m_position, inTranslation);
    }

    void Actor::setRelativeRotation(const glm::vec3& inRotation)
    {
        m_transform.rotation += inRotation;

       setRotation(m_position, inRotation);
    }

    void Actor::setRelativeScale(const glm::vec3& inScale)
    {
        m_transform.scale += inScale;

        setScale(m_position, inScale);
    }

    void Actor::setAbsoluteTranslation(const glm::vec3& inTranslation)
    {
        m_transform.translation = inTranslation;

        setTranslation(glm::mat4(1.0f), inTranslation);
    }

    void Actor::setAbsoluteRotation(const glm::vec3& inRotation)
    {
        m_transform.rotation = inRotation;

        setRotation(glm::mat4(0.0f), inRotation);
    }

    void Actor::setAbsoluteScale(const glm::vec3& inScale)
    {
        m_transform.scale = inScale;

        setScale(glm::mat4(1.0f), inScale);
    }

    bool Actor::hasMesh()
    {
        return m_mesh.type == static_cast<uint8_t>(Box::Type::Mesh);
    }

    Box::Instance Actor::getMesh()
    {
        return m_mesh;
    }

    void Actor::setMesh(const Box::Instance& inMesh)
    {
        m_mesh = inMesh;
    }

    void Actor::setTranslation(const glm::mat4& inBase, const glm::vec3& inTranslation)
    {
        m_position *= glm::translate(inBase, inTranslation);
    }

    void Actor::setRotation(const glm::mat4& inBase, const glm::vec3& inRotation)
    {
        glm::vec3 radianAngles = glm::radians(inRotation);

        m_position = glm::rotate(
            inBase,
            radianAngles.x,
            glm::vec3(0.0f, 1.0f, 0.0f)
        );
        m_position = glm::rotate(
            inBase,
            radianAngles.y,
            glm::vec3(0.0f, 0.0f, 1.0f)
        );
        m_position = glm::rotate(
            inBase,
            radianAngles.z,
            glm::vec3(1.0f, 0.0f, 0.0f)
        );
    }

    void Actor::setScale(const glm::mat4& inBase, const glm::vec3& inScale)
    {
        m_position *= glm::scale(inBase, inScale);
    }
}