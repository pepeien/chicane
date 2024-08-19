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

        m_controller->bindMouseMotionEvent(
            [this](const SDL_MouseMotionEvent& inEvent)
            {
                if (!isWindowFocused())
                {
                    return;
                }

                m_camera->addPitch(inEvent.yrel);
                m_camera->addYaw(inEvent.xrel);
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

    void CameraActor::moveForward(bool isKeyDown)
    {
        if (!isKeyDown || !isWindowFocused())
        {
            return;
        }

        Vec<float>::Three position = m_camera->getPosition();
        position.x += MOVEMENT_COEFFICIENT;

        m_camera->setPosition(position);
    }

    void CameraActor::moveBackward(bool isKeyDown)
    {
        if (!isKeyDown || !isWindowFocused())
        {
            return;
        }

        Vec<float>::Three position = m_camera->getPosition();
        position.x -= MOVEMENT_COEFFICIENT;

        m_camera->setPosition(position);
    }

    void CameraActor::moveLeft(bool isKeyDown)
    {
        if (!isKeyDown || !isWindowFocused())
        {
            return;
        }

        Vec<float>::Three position = m_camera->getPosition();
        position.y -= MOVEMENT_COEFFICIENT;

        m_camera->setPosition(position);
    }

    void CameraActor::moveRight(bool isKeyDown)
    {
        if (!isKeyDown || !isWindowFocused())
        {
            return;
        }

        Vec<float>::Three position = m_camera->getPosition();
        position.y += MOVEMENT_COEFFICIENT;

        m_camera->setPosition(position);
    }
}