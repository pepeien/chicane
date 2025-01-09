#include "Chicane/Game/Transformable/Actor/Pawn/Instance.hpp"

#include "Chicane/Game/Controller/Instance.hpp"

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
}