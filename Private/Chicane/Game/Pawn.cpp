#include "Chicane/Game/Pawn.hpp"

#include "Chicane/Game/Controller.hpp"

namespace Chicane
{
    Pawn::Pawn()
        : Actor(),
        m_controller(nullptr)
    {}

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
        m_controller = nullptr;
    }
}