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
        addAbsoluteTranslation(0.0f, 0.0f, 10.0f);

        m_physics = getScene()->createComponent<CPhysics>();
        m_physics->attachTo(this);
        m_physics->activate();
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
}