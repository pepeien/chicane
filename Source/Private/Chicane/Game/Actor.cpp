#include "Chicane/Game/Actor.hpp"

#include "Chicane/Game/Components/Actor.hpp"

namespace Chicane
{
    Actor::Actor()
        : m_bCanTick(false),
        m_position(Mat<float>::Four(1.0f)),
        m_transform({})
    {}

    bool Actor::canTick()
    {
        return m_bCanTick;
    }

    void Actor::setCanTick(bool bInCanTick)
    {
        m_bCanTick = bInCanTick;
    }

    Mat<float>::Four Actor::getPosition()
    {
        return m_position;
    }

    void Actor::setRelativeTranslation(const Vec<float>::Three& inTranslation)
    {
        m_transform.translation += inTranslation;

        setTranslation(m_position, inTranslation);
    }

    void Actor::setRelativeRotation(const Vec<float>::Three& inRotation)
    {
        m_transform.rotation += inRotation;

       setRotation(m_position, inRotation);
    }

    void Actor::setRelativeScale(const Vec<float>::Three& inScale)
    {
        m_transform.scale += inScale;

        setScale(m_position, inScale);
    }

    void Actor::setAbsoluteTranslation(const Vec<float>::Three& inTranslation)
    {
        m_transform.translation = inTranslation;

        setTranslation(Mat<float>::Four(1.0f), inTranslation);
    }

    void Actor::setAbsoluteRotation(const Vec<float>::Three& inRotation)
    {
        m_transform.rotation = inRotation;

        setRotation(Mat<float>::Four(0.0f), inRotation);
    }

    void Actor::setAbsoluteScale(const Vec<float>::Three& inScale)
    {
        m_transform.scale = inScale;

        setScale(Mat<float>::Four(1.0f), inScale);
    }

    bool Actor::hasMesh()
    {
        return m_mesh.type == Box::Type::Mesh;
    }

    Box::Instance Actor::getMesh()
    {
        return m_mesh;
    }

    void Actor::setMesh(const Box::Instance& inMesh)
    {
        m_mesh = inMesh;
    }

    void Actor::setTranslation(const Mat<float>::Four& inBase, const Vec<float>::Three& inTranslation)
    {
        m_position *= glm::translate(inBase, inTranslation);
    }

    void Actor::setRotation(const Mat<float>::Four& inBase, const Vec<float>::Three& inRotation)
    {
        Vec<float>::Three radianAngles = glm::radians(inRotation);

        m_position = glm::rotate(
            inBase,
            radianAngles.x,
            Vec<float>::Three(1.0f, 0.0f, 0.0f)
        );
        m_position = glm::rotate(
            inBase,
            radianAngles.y,
            Vec<float>::Three(0.0f, 1.0f, 0.0f)
        );
        m_position = glm::rotate(
            inBase,
            radianAngles.z,
            Vec<float>::Three(0.0f, 0.0f, 1.0f)
        );
    }

    void Actor::setScale(const Mat<float>::Four& inBase, const Vec<float>::Three& inScale)
    {
        m_position *= glm::scale(inBase, inScale);
    }
}