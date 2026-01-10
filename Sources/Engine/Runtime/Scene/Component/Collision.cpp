#include "Chicane/Runtime/Scene/Component/Collision.hpp"

#include "Chicane/Box/Asset/Model/Manager.hpp"
#include "Chicane/Runtime/Application.hpp"

namespace Chicane
{
    CCollision::CCollision()
        : Super(),
          m_bCanCollide(true),
          m_shape({})
    {}

    void CCollision::onTick(float inDeltaTime)
    {
        if (!canCollide())
        {
            return;
        }

        Actor* parent = static_cast<Actor*>(m_parent);

        if (!parent)
        {
            return;
        }

        for (CCollision* collider : Application::getScene()->getComponents<CCollision>())
        {
            if (!isColliding(collider))
            {
                continue;
            }

            Actor* colliderParent = collider->getParent<Actor>();

            if (parent == colliderParent)
            {
                continue;
            }

            parent->collideWith(colliderParent);
        }
    }

    bool CCollision::canCollide() const
    {
        return m_bCanCollide && m_bIsActive;
    }

    void CCollision::setCanCollide(bool inValue)
    {
        m_bCanCollide = inValue;
    }

    const Box::ModelExtracted& CCollision::getShape() const
    {
        return m_shape;
    }

    void CCollision::setShape(const Box::Model* inAsset)
    {
        if (!inAsset)
        {
            return;
        }

        setShape(inAsset->getId());
    }

    void CCollision::setShape(const String& inAsset)
    {
        m_shape = {};

        if (!Box::getModelManager()->isLoaded(inAsset))
        {
            return;
        }

        m_shape = Box::getModelManager()->getInstance(inAsset);
    }

    bool CCollision::isColliding(CCollision* inOther) const
    {
        if (!inOther || this == inOther)
        {
            return false;
        }

        if (!canCollide() || !inOther->canCollide())
        {
            return false;
        }

        const Box::ModelExtracted& otherShape = inOther->getShape();

        for (std::uint32_t i = 0; i < otherShape.indices.size(); i++)
        {
            // const Vec3& otherPoint = otherShape.vertices.at(i);

            for (std::uint32_t j = 0; j < m_shape.indices.size(); j++)
            {
                // const Vec3& point = m_shape.vertices.at(j);

                // const bool bIsWithinX = otherPoint.x >= min.x && otherPoint.x
                // <= max.x; const bool bIsWithinY = otherPoint.y >= min.y &&
                // otherPoint.y <= max.y; const bool bIsWithinZ = otherPoint.z
                // >= min.z && otherPoint.z <= max.z;
            }
        }

        return false;
    }
}