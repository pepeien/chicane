#pragma once

#include "Chicane/Base.hpp"
#include "Chicane/Game/Transformable/Actor/Pawn/Instance.hpp"
#include "Chicane/Game/Transformable/Component/Camera.hpp"

namespace Chicane
{
    class ACameraPawn : public APawn
    {
    public:
        ACameraPawn();
        virtual ~ACameraPawn() = default;

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
        CCamera* m_camera;
    };
}