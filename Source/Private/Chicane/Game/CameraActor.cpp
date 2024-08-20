#include "Chicane/Game/CameraActor.hpp"

#include "Chicane/Core.hpp"
#include "Chicane/Game/Controller.hpp"
#include "Chicane/Game/Components/Camera.hpp"

constexpr float MOVEMENT_COEFFICIENT = 5.0f;

namespace Chicane
{
    CameraActor::CameraActor()
        : Pawn()
    {
        m_camera = new CameraComponent();
    }

    void CameraActor::onPossession()
    {
        setupMouseInputs();
        setupKeyboardInputs();
        setupControllerInputs();
    }

    void CameraActor::setupMouseInputs()
    {
        m_controller->bindMouseMotionEvent(
            [this](const SDL_MouseMotionEvent& inEvent)
            {
                look(
                    inEvent.xrel,
                    -inEvent.yrel
                );
            }
        );
        m_controller->bindMouseButtonEvent(
            SDL_BUTTON_LEFT,
            [](bool isKeyDown)
            {
                if (!isKeyDown)
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
                CameraActor::moveUp,
                this,
                std::placeholders::_1
            )
        );
        m_controller->bindKeyboardButtonEvent(
            SDL_SCANCODE_LSHIFT,
            std::bind(
                CameraActor::moveDown,
                this,
                std::placeholders::_1
            )
        );
        m_controller->bindKeyboardButtonEvent(
            SDL_SCANCODE_W,
            std::bind(
                CameraActor::moveForward,
                this,
                std::placeholders::_1
            )
        );
        m_controller->bindKeyboardButtonEvent(
            SDL_SCANCODE_S,
            std::bind(
                CameraActor::moveBackward,
                this,
                std::placeholders::_1
            )
        );
        m_controller->bindKeyboardButtonEvent(
            SDL_SCANCODE_A,
            std::bind(
                CameraActor::moveLeft,
                this,
                std::placeholders::_1
            )
        );
        m_controller->bindKeyboardButtonEvent(
            SDL_SCANCODE_D,
            std::bind(
                CameraActor::moveRight,
                this,
                std::placeholders::_1
            )
        );
    }

    void CameraActor::setupControllerInputs()
    {
        // Motion
        m_controller->bindControllerMotionEvent(
            [this](const SDL_ControllerAxisEvent& inEvent)
            {
                std::uint16_t axisValue = inEvent.value;

                look(
                    1.0f,
                    0.0f
                );
            }
        );

        // Button
        m_controller->bindControllerButtonEvent(
            SDL_CONTROLLER_BUTTON_A,
            [](bool isKeyDown)
            {
                if (!isKeyDown)
                {
                    return;
                }

                setWindowFocus(!isWindowFocused());
            }
        );
    }

    void CameraActor::look(float inX, float inY)
    {
        if (!isWindowFocused())
        {
            return;
        }

        m_camera->addRotation(
            0.0f,
            inX,
            inY
        );
    }

    void CameraActor::moveUp(bool isKeyDown)
    {
        if (!isKeyDown || !isWindowFocused())
        {
            return;
        }

        m_camera->addTranslation(0.0f, 0.0f, MOVEMENT_COEFFICIENT);
    }

    void CameraActor::moveDown(bool isKeyDown)
    {
        if (!isKeyDown || !isWindowFocused())
        {
            return;
        }

        m_camera->addTranslation(0.0f, 0.0f, -MOVEMENT_COEFFICIENT);
    }

    void CameraActor::moveForward(bool isKeyDown)
    {
        if (!isKeyDown || !isWindowFocused())
        {
            return;
        }

        m_camera->addTranslation(
            MOVEMENT_COEFFICIENT,
            0.0f,
            0.0f
        );
    }

    void CameraActor::moveBackward(bool isKeyDown)
    {
        if (!isKeyDown || !isWindowFocused())
        {
            return;
        }

        m_camera->addTranslation(
            -MOVEMENT_COEFFICIENT,
            0.0f,
            0.0f
        );
    }

    void CameraActor::moveLeft(bool isKeyDown)
    {
        if (!isKeyDown || !isWindowFocused())
        {
            return;
        }

        m_camera->addTranslation(
            0.0f,
            -MOVEMENT_COEFFICIENT,
            0.0f
        );
    }

    void CameraActor::moveRight(bool isKeyDown)
    {
        if (!isKeyDown || !isWindowFocused())
        {
            return;
        }

        m_camera->addTranslation(
            0.0f,
            MOVEMENT_COEFFICIENT,
            0.0f
        );
    }
}