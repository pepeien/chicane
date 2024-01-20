#include "Pawn.hpp"
#include "Controller.hpp"

namespace Engine
{
    Controller* Pawn::getController()
    {
        return m_controller;
    }

    bool Pawn::isPossessed()
    {
        return m_controller != nullptr;
    }

    void Pawn::getPossesedBy(Controller* inController)
    {
        m_controller = inController;
    }

    void Pawn::getUnpossessed()
    {
        m_controller == nullptr;
    }
}