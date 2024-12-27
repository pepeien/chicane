#include "Chicane/Game/Transformable/Actor/Pawn/CameraPawn.hpp"

#include "Chicane/Core.hpp"
#include "Chicane/Game/Controller.hpp"

constexpr float MOVEMENT_COEFFICIENT = 1.5f;

namespace Chicane
{
    CameraPawn::CameraPawn()
        : Pawn(),
        m_camera(new CameraComponent())
    {
        m_camera->attachTo(this);
    }

    void CameraPawn::onControlAttachment()
    {
        setupMouseInputs();
        setupKeyboardInputs();
        setupControllerInputs();

        m_camera->activate();
    }

    void CameraPawn::setupMouseInputs()
    {
        m_controller->bindMouseMotionEvent(
            [&](const SDL_MouseMotionEvent& inEvent)
            {
                if (!isWindowFocused())
                {
                    return;
                }

                setRelativeRotation(
                    Vec<3, float>(
                        0.0f,
                        -inEvent.xrel * 0.5f,
                        -inEvent.yrel * 0.5f
                    )
                );
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

                setWindowFocus(!isWindowFocused());
            }
        );
    }

    void CameraPawn::setupKeyboardInputs()
    {
        m_controller->bindKeyboardButtonEvent(
            SDL_SCANCODE_SPACE,
            std::bind(
                &CameraPawn::moveUp,
                this,
                std::placeholders::_1
            )
        );
        m_controller->bindKeyboardButtonEvent(
            SDL_SCANCODE_LCTRL,
            std::bind(
                &CameraPawn::moveDown,
                this,
                std::placeholders::_1
            )
        );
        m_controller->bindKeyboardButtonEvent(
            SDL_SCANCODE_RCTRL,
            std::bind(
                &CameraPawn::moveDown,
                this,
                std::placeholders::_1
            )
        );
        m_controller->bindKeyboardButtonEvent(
            SDL_SCANCODE_W,
            std::bind(
                &CameraPawn::moveForward,
                this,
                std::placeholders::_1
            )
        );
        m_controller->bindKeyboardButtonEvent(
            SDL_SCANCODE_S,
            std::bind(
                &CameraPawn::moveBackward,
                this,
                std::placeholders::_1
            )
        );
        m_controller->bindKeyboardButtonEvent(
            SDL_SCANCODE_A,
            std::bind(
                &CameraPawn::moveLeft,
                this,
                std::placeholders::_1
            )
        );
        m_controller->bindKeyboardButtonEvent(
            SDL_SCANCODE_D,
            std::bind(
                &CameraPawn::moveRight,
                this,
                std::placeholders::_1
            )
        );
    }

    void CameraPawn::setupControllerInputs()
    {
        m_controller->bindControllerButtonEvent(
            SDL_GAMEPAD_BUTTON_SOUTH,
            [](bool bInIsKeyDown)
            {
                if (!bInIsKeyDown || !isWindowFocused())
                {
                    return;
                }

                setWindowFocus(!isWindowFocused());
            }
        );
    }

    void CameraPawn::moveUp(bool bInIsKeyDown)
    {
        if (!bInIsKeyDown || !isWindowFocused())
        {
            return;
        }

        setRelativeTranslation(
            Vec<3, float>(
                0.0f,
                0.0f,
                MOVEMENT_COEFFICIENT
            )
        );
    }

    void CameraPawn::moveDown(bool bInIsKeyDown)
    {
        if (!bInIsKeyDown || !isWindowFocused())
        {
            return;
        }

        setRelativeTranslation(
            Vec<3, float>(
                0.0f,
                0.0f,
                -MOVEMENT_COEFFICIENT
            )
        );
    }

    void CameraPawn::moveForward(bool bInIsKeyDown)
    {
        if (!bInIsKeyDown || !isWindowFocused())
        {
            return;
        }

        setRelativeTranslation(getForward() * MOVEMENT_COEFFICIENT);
    }

    void CameraPawn::moveBackward(bool bInIsKeyDown)
    {
        if (!bInIsKeyDown || !isWindowFocused())
        {
            return;
        }

        setRelativeTranslation(getForward() * -MOVEMENT_COEFFICIENT);
    }

    void CameraPawn::moveLeft(bool bInIsKeyDown)
    {
        if (!bInIsKeyDown || !isWindowFocused())
        {
            return;
        }

        setRelativeTranslation(getRight() * -MOVEMENT_COEFFICIENT);
    }

    void CameraPawn::moveRight(bool bInIsKeyDown)
    {
        if (!bInIsKeyDown || !isWindowFocused())
        {
            return;
        }

        setRelativeTranslation(getRight() * MOVEMENT_COEFFICIENT);
    }
}