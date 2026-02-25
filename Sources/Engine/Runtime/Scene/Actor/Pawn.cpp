#include "Chicane/Runtime/Scene/Actor/Pawn.hpp"

#include "Chicane/Runtime/Controller.hpp"
#include "Chicane/Runtime/Scene.hpp"

namespace Chicane
{
    APawn::APawn()
        : Actor(),
          m_controller(nullptr),
          m_physics(nullptr)
    {
        setCanCollide(true);
    }

    void APawn::onLoad()
    {
        m_physics = getScene()->createComponent<CPhysics>();
        m_physics->setShape(Kerb::BodyShape::Capsule);
        m_physics->setMotion(Kerb::MotionType::Dynamic);
        m_physics->attachTo(this);
    }

    bool APawn::isControlled() const
    {
        return m_controller != nullptr;
    }

    void APawn::enablePhysics()
    {
        if (!m_physics)
        {
            return;
        }

        m_physics->activate();
    }

    void APawn::disabledPhysics()
    {
        if (!m_physics)
        {
            return;
        }

        m_physics->deactivate();
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
}