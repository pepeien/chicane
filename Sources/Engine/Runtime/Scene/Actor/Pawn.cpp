#include "Chicane/Runtime/Scene/Actor/Pawn.hpp"

#include "Chicane/Runtime/Controller.hpp"
#include "Chicane/Runtime/Scene.hpp"

namespace Chicane
{
    APawn::APawn()
        : Actor(),
          m_controller(nullptr),
          m_physicsComponent(nullptr)
    {
        setCanCollide(true);
    }

    void APawn::onLoad()
    {
        m_physicsComponent = getScene()->createComponent<CPhysics>();
        m_physicsComponent->attachTo(this);
        m_physicsComponent->activate();
    }

    bool APawn::isControlled() const
    {
        return m_controller != nullptr;
    }

    void APawn::attachController(Controller* inController)
    {
        m_controller = inController;

        onControlAttachment();
    }

    void APawn::deattachController()
    {
        m_controller = nullptr;

        onControleDeattachment();
    }

    CPhysics* APawn::getPhysicsComponent() const
    {
        return m_physicsComponent;
    }
}