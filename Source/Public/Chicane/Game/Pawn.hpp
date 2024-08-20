#pragma once

#include "Chicane/Base.hpp"
#include "Chicane/Game/Actor.hpp"

namespace Chicane
{
    class Controller;

    class Pawn : public Actor
    {
    public:
        Pawn();

    public:
        template<class T = Controller>
        T* getController()
        {
            return dynamic_cast<T*>(m_controller);
        }

        bool isControlled();
        void attachController(Controller* inController);
        void deattachController();

        void addControllerRollInput(float inValue);
        void addControllerYawInput(float inValue);
        void addControllerPitchInput(float inValue);

    protected:
        virtual void onControlAttachment() {};
        virtual void onControleDeattachment() {};

    protected:
        Controller* m_controller;
    };
}