#pragma once

#include "Chicane/Core/Reflection.hpp"

#include "Chicane/Runtime.hpp"
#include "Chicane/Runtime/Scene/Actor.hpp"
#include "Chicane/Runtime/Scene/Component/Physics.hpp"

namespace Chicane
{
    class Controller;

    CH_TYPE(Manual)
    class CHICANE_RUNTIME APawn : public Actor
    {
        friend Controller;

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

    protected:
        void enablePhysics();
        void disabledPhysics();

    private:
        void attachController(Controller* inController);
        void deattachController();

    protected:
        Controller* m_controller;

        CPhysics*   m_physics;
    };
}