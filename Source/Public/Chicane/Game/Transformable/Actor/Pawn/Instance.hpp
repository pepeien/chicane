#pragma once

#include "Chicane/Base.hpp"
#include "Chicane/Game/Transformable/Actor/Instance.hpp"

namespace Chicane
{
    class Controller;

    class Pawn : public Actor
    {
    public:
        Pawn();
        virtual ~Pawn() = default;

    protected:
        virtual void onControlAttachment() { return; };
        virtual void onControleDeattachment() { return; };

    public:
        template<class T = Controller>
        T* getController() const
        {
            return dynamic_cast<T*>(m_controller);
        }

        bool isControlled() const;
        void attachController(Controller* inController);
        void deattachController();

    protected:
        Controller* m_controller;
    };
}