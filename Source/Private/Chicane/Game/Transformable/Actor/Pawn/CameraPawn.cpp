#include "Chicane/Game/Transformable/Actor/Pawn/CameraPawn.hpp"

#include "Chicane/Application.hpp"
#include "Chicane/Core.hpp"
#include "Chicane/Game/Controller/Instance.hpp"

constexpr float MOVEMENT_COEFFICIENT = 1.5f;

namespace Chicane
{
    ACameraPawn::ACameraPawn()
        : APawn(),
        m_camera(new CCamera())
    {
        m_camera->attachTo(this);
    }

    void ACameraPawn::onControlAttachment()
    {
        setupMouseInputs();
        setupKeyboardInputs();
        setupControllerInputs();

        m_camera->activate();
    }

    void ACameraPawn::setupMouseInputs()
    {
        m_controller->bindMouseMotionEvent(
            [&](const SDL_MouseMotionEvent& inEvent)
            {
                if (!Application::getWindow()->isFocused())
                {
                    return;
                }

                addAbsoluteRotation(-inEvent.yrel * 0.5f, 0.0f, -inEvent.xrel * 0.5f);
            }
        );

        m_controller->bindMouseButtonEvent(
            SDL_BUTTON_RIGHT,
            [](bool bInIsButtonPressed)
            {
                if (!bInIsButtonPressed)
                {
                    return;
                }

                Application::getWindow()->switchFocus();
            }
        );
    }

    void ACameraPawn::setupKeyboardInputs()
    {
        m_controller->bindKeyboardButtonEvent(
            SDL_SCANCODE_SPACE,
            std::bind(
                &ACameraPawn::moveUp,
                this,
                std::placeholders::_1
            )
        );
        m_controller->bindKeyboardButtonEvent(
            SDL_SCANCODE_LCTRL,
            std::bind(
                &ACameraPawn::moveDown,
                this,
                std::placeholders::_1
            )
        );
        m_controller->bindKeyboardButtonEvent(
            SDL_SCANCODE_RCTRL,
            std::bind(
                &ACameraPawn::moveDown,
                this,
                std::placeholders::_1
            )
        );
        m_controller->bindKeyboardButtonEvent(
            SDL_SCANCODE_W,
            std::bind(
                &ACameraPawn::moveForward,
                this,
                std::placeholders::_1
            )
        );
        m_controller->bindKeyboardButtonEvent(
            SDL_SCANCODE_S,
            std::bind(
                &ACameraPawn::moveBackward,
                this,
                std::placeholders::_1
            )
        );
        m_controller->bindKeyboardButtonEvent(
            SDL_SCANCODE_A,
            std::bind(
                &ACameraPawn::moveLeft,
                this,
                std::placeholders::_1
            )
        );
        m_controller->bindKeyboardButtonEvent(
            SDL_SCANCODE_D,
            std::bind(
                &ACameraPawn::moveRight,
                this,
                std::placeholders::_1
            )
        );
    }

    void ACameraPawn::setupControllerInputs()
    {
        m_controller->bindControllerButtonEvent(
            SDL_GAMEPAD_BUTTON_SOUTH,
            [](bool bInIsKeyDown)
            {
                if (!bInIsKeyDown || !Application::getWindow()->isFocused())
                {
                    return;
                }

                Application::getWindow()->switchFocus();
            }
        );
    }

    void ACameraPawn::moveUp(bool bInIsKeyDown)
    {
        if (!bInIsKeyDown || !Application::getWindow()->isFocused())
        {
            return;
        }

        addAbsoluteTranslation(0.0f, 0.0f, MOVEMENT_COEFFICIENT);
    }

    void ACameraPawn::moveDown(bool bInIsKeyDown)
    {
        if (!bInIsKeyDown || !Application::getWindow()->isFocused())
        {
            return;
        }

        addAbsoluteTranslation(0.0f,  0.0f, -MOVEMENT_COEFFICIENT);
    }

    void ACameraPawn::moveForward(bool bInIsKeyDown)
    {
        if (!bInIsKeyDown || !Application::getWindow()->isFocused())
        {
            return;
        }

        addAbsoluteTranslation(getForward() * MOVEMENT_COEFFICIENT);
    }

    void ACameraPawn::moveBackward(bool bInIsKeyDown)
    {
        if (!bInIsKeyDown || !Application::getWindow()->isFocused())
        {
            return;
        }

        addAbsoluteTranslation(getForward() * -MOVEMENT_COEFFICIENT);
    }

    void ACameraPawn::moveLeft(bool bInIsKeyDown)
    {
        if (!bInIsKeyDown || !Application::getWindow()->isFocused())
        {
            return;
        }

        addAbsoluteTranslation(getRight() * -MOVEMENT_COEFFICIENT);
    }

    void ACameraPawn::moveRight(bool bInIsKeyDown)
    {
        if (!bInIsKeyDown || !Application::getWindow()->isFocused())
        {
            return;
        }

        addAbsoluteTranslation(getRight() * MOVEMENT_COEFFICIENT);
    }
}