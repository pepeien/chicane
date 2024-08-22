#include "Chicane/Game/Pawn.hpp"

#include "Chicane/Game/Components/Camera.hpp"
#include "Chicane/Game/Controller.hpp"

namespace Chicane
{
    Pawn::Pawn()
        : Actor(),
        m_controller(nullptr)
    {}

    bool Pawn::isControlled() const
    {
        return m_controller != nullptr;
    }

    void Pawn::attachController(Controller* inController)
    {
        m_controller = inController;

        onControlAttachment();
    }

    void Pawn::deattachController()
    {
        m_controller = nullptr;

        onControleDeattachment();
    }

    void Pawn::addControllerRollInput(float inValue)
    {
        if (!hasCamera())
        {
            return;
        }

        m_camera->addRotation(inValue, 0.0f, 0.0f);
    }

    void Pawn::addControllerYawInput(float inValue)
    {
        if (!hasCamera())
        {
            return;
        }

        m_camera->addRotation(0.0f, inValue, 0.0f);
    }

    void Pawn::addControllerPitchInput(float inValue)
    {
        if (!hasCamera())
        {
            return;
        }

        m_camera->addRotation(0.0f, 0.0f, inValue);
    }
}