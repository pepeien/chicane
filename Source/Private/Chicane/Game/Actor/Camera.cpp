#include "Chicane/Game/Actor/Camera.hpp"

#include "Chicane/Core.hpp"
#include "Chicane/Game/Controller.hpp"

constexpr float MOVEMENT_COEFFICIENT = 1.0f;

namespace Chicane
{
    CameraActor::CameraActor()
        : Pawn(),
        m_camera(std::make_unique<CameraComponent>())
    {
        m_camera->attachTo(this);
    }

    void CameraActor::onControlAttachment()
    {
        setupMouseInputs();
        setupKeyboardInputs();
        setupControllerInputs();

        m_camera->activate();
    }

    void CameraActor::setupMouseInputs()
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
            [](bool isButtonPressed)
            {
                if (!isButtonPressed)
                {
                    return;
                }

                setWindowFocus(!isWindowFocused());
            }
        );
    }

    void CameraActor::setupKeyboardInputs()
    {
        m_controller->bindKeyboardButtonEvent(
            SDL_SCANCODE_SPACE,
            std::bind(
                &CameraActor::moveUp,
                this,
                std::placeholders::_1
            )
        );
        m_controller->bindKeyboardButtonEvent(
            SDL_SCANCODE_LCTRL,
            std::bind(
                &CameraActor::moveDown,
                this,
                std::placeholders::_1
            )
        );
        m_controller->bindKeyboardButtonEvent(
            SDL_SCANCODE_RCTRL,
            std::bind(
                &CameraActor::moveDown,
                this,
                std::placeholders::_1
            )
        );
        m_controller->bindKeyboardButtonEvent(
            SDL_SCANCODE_W,
            std::bind(
                &CameraActor::moveForward,
                this,
                std::placeholders::_1
            )
        );
        m_controller->bindKeyboardButtonEvent(
            SDL_SCANCODE_S,
            std::bind(
                &CameraActor::moveBackward,
                this,
                std::placeholders::_1
            )
        );
        m_controller->bindKeyboardButtonEvent(
            SDL_SCANCODE_A,
            std::bind(
                &CameraActor::moveLeft,
                this,
                std::placeholders::_1
            )
        );
        m_controller->bindKeyboardButtonEvent(
            SDL_SCANCODE_D,
            std::bind(
                &CameraActor::moveRight,
                this,
                std::placeholders::_1
            )
        );
    }

    void CameraActor::setupControllerInputs()
    {
        m_controller->bindControllerButtonEvent(
            SDL_CONTROLLER_BUTTON_A,
            [](bool isKeyDown)
            {
                if (!isKeyDown || !isWindowFocused())
                {
                    return;
                }

                setWindowFocus(!isWindowFocused());
            }
        );
    }

    void CameraActor::moveUp(bool isKeyDown)
    {
        if (!isKeyDown || !isWindowFocused())
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

    void CameraActor::moveDown(bool isKeyDown)
    {
        if (!isKeyDown || !isWindowFocused())
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

    void CameraActor::moveForward(bool isKeyDown)
    {
        if (!isKeyDown || !isWindowFocused())
        {
            return;
        }

        setRelativeTranslation(
            Vec<3, float>(
                0.0f,
                MOVEMENT_COEFFICIENT,
                0.0f
            )
        );
    }

    void CameraActor::moveBackward(bool isKeyDown)
    {
        if (!isKeyDown || !isWindowFocused())
        {
            return;
        }

        setRelativeTranslation(
            Vec<3, float>(
                0.0f,
                -MOVEMENT_COEFFICIENT,
                0.0f
            )
        );
    }

    void CameraActor::moveLeft(bool isKeyDown)
    {
        if (!isKeyDown || !isWindowFocused())
        {
            return;
        }

        setRelativeTranslation(
            Vec<3, float>(
                -MOVEMENT_COEFFICIENT,
                0.0f,
                0.0f
            )
        );
    }

    void CameraActor::moveRight(bool isKeyDown)
    {
        if (!isKeyDown || !isWindowFocused())
        {
            return;
        }

        setRelativeTranslation(
            Vec<3, float>(
                MOVEMENT_COEFFICIENT,
                0.0f,
                0.0f
            )
        );
    }
}