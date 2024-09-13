#pragma once

#include "Chicane/Base.hpp"
#include "Chicane/Game/Actor/Pawn.hpp"
#include "Chicane/Game/Actor/Component/Camera.hpp"

namespace Chicane
{
    class CameraActor : public Pawn
    {
    public:
        CameraActor();
        virtual ~CameraActor() = default;

    protected:
        void onControlAttachment() override;

    private:
        void setupMouseInputs();
        void setupKeyboardInputs();
        void setupControllerInputs();

        void moveUp(bool bInIsKeyDown);
        void moveDown(bool bInIsKeyDown);
        void moveForward(bool bInIsKeyDown);
        void moveBackward(bool bInIsKeyDown);
        void moveLeft(bool bInIsKeyDown);
        void moveRight(bool bInIsKeyDown);

    protected:
        CameraComponent* m_camera;
    };
}