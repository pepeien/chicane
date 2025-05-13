#include "Chicane/Runtime/Game/Transformable/Actor/Pawn/Instance.hpp"

#include "Chicane/Runtime/Application.hpp"
#include "Chicane/Runtime/Game/Controller/Instance.hpp"

namespace Chicane
{
    APawn::APawn()
        : Actor(),
        m_controller(nullptr),
        m_physicsComponent(nullptr)
    {
        setCanCollide(true);

        m_physicsComponent = Application::getLevel()->createComponent<CPhysics>();
        m_physicsComponent->attachTo(this);
        m_physicsComponent->activate();
    }

    bool APawn::isControlled() const
    {
        return m_controller != nullptr;
    }

    void APawn::attachController(Controller::Instance* inController)
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