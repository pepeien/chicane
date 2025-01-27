#include "Chicane/Game/Transformable/Actor/Pawn/Instance.hpp"

#include "Chicane/Game/Controller/Instance.hpp"

namespace Chicane
{
    APawn::APawn()
        : Actor(),
        m_controller(nullptr)
    {}

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