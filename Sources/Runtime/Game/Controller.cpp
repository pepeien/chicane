#include "Runtime/Game/Controller.hpp"

#include "Runtime/Game/Pawn.hpp"

namespace Chicane
{
    Controller::Controller()
        : m_pawn(nullptr)
    {}

    void Controller::possess(Pawn* inPawn)
    {
        if (inPawn == nullptr)
        {
            LOG_WARNING("Pawn is null");

            return;
        }

        if (inPawn->isPossessed())
        {
            LOG_WARNING("This pawn is currently possesed");

            return;
        }

        inPawn->getPossesedBy(this);

        m_pawn = inPawn;
    }

    void Controller::unPossess()
    {
        if (m_pawn == nullptr)
        {
            LOG_INFO("The controller doesn't possess a Pawn");

            return;
        }

        m_pawn->getUnpossessed();
        m_pawn = nullptr;
    }

    void Controller::bindEvent(SDL_Scancode inScanCode, std::function<void()> inEvent)
    {
        events.insert(
            std::make_pair(
                inScanCode,
                inEvent
            )
        );
    }

    void Controller::onEvent(SDL_Scancode inScanCode)
    {
        auto foundEvent = events.find(inScanCode);

        if (foundEvent == events.end())
        {
            return;
        }

        foundEvent->second();
    }
}