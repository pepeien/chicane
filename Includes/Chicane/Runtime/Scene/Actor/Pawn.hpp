#pragma once

#include "Chicane/Runtime.hpp"
#include "Chicane/Runtime/Scene/Actor.hpp"
#include "Chicane/Runtime/Scene/Component/Physics.hpp"

namespace Chicane
{
    class Controller;

    class CHICANE_RUNTIME APawn : public Actor
    {
    public:
        APawn();

    protected:
        virtual void onControlAttachment() { return; }
        virtual void onControleDeattachment() { return; }

    protected:
        void onLoad() override;

    public:
        bool isControlled() const;
        template <class T = Controller>
        T* getController() const
        {
            return dynamic_cast<T*>(m_controller);
        }
        void attachController(Controller* inController);
        void deattachController();

        CPhysics* getPhysicsComponent() const;

    protected:
        Controller* m_controller;

        CPhysics*   m_physicsComponent;
    };
}