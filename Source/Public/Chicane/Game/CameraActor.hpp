#pragma once

#include "Chicane/Base.hpp"
#include "Chicane/Game/Pawn.hpp"

namespace Chicane
{
    class CameraActor : public Pawn
    {
    public:
        CameraActor();
    
    public:
        void onPossession() override;
    
    private:
        void setupMouseInputs();
        void setupKeyboardInputs();
        void setupControllerInputs();

        void look(float inX, float inY);

        void moveUp(bool isKeyDown);
        void moveDown(bool isKeyDown);
        void moveForward(bool isKeyDown);
        void moveBackward(bool isKeyDown);
        void moveLeft(bool isKeyDown);
        void moveRight(bool isKeyDown);
    };
}