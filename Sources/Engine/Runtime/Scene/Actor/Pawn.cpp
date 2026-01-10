#include "Chicane/Runtime/Scene/Actor/Pawn.hpp"

#include "Chicane/Runtime/Application.hpp"
#include "Chicane/Runtime/Controller.hpp"

namespace Chicane
{
    APawn::APawn()
        : Super(),
          m_controller(nullptr),
          m_physicsComponent(nullptr)
    {
        setCanCollide(true);

        m_physicsComponent =
            Application::getScene()->createComponent<CPhysics>();
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