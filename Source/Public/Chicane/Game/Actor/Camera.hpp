#pragma once

#include "Chicane/Base.hpp"
#include "Chicane/Game/Actor/Pawn.hpp"
#include "Chicane/Game/Actor/Component/Camera.hpp"
#include "Chicane/Game/Actor/Component/Mesh.hpp"

namespace Chicane
{
    class CameraActor : public Pawn
    {
    public:
        CameraActor();

    protected:
        void onControlAttachment() override;

    private:
        void setupMouseInputs();
        void setupKeyboardInputs();
        void setupControllerInputs();

        void moveUp(bool isKeyDown);
        void moveDown(bool isKeyDown);
        void moveForward(bool isKeyDown);
        void moveBackward(bool isKeyDown);
        void moveLeft(bool isKeyDown);
        void moveRight(bool isKeyDown);

    protected:
        std::unique_ptr<CameraComponent> m_camera;
        std::unique_ptr<MeshComponent> m_mesh;
    };
}