#pragma once

#include "Chicane/Base.hpp"
#include "Chicane/Game/Transformable/Actor/Instance.hpp"
#include "Chicane/Game/Transformable/Component/Physics.hpp"

namespace Chicane
{
    namespace Controller
    {
        class Instance;
    }

    class APawn : public Actor
    {
    public:
        APawn();
        virtual ~APawn() = default;

    protected:
        virtual void onControlAttachment() { return; };
        virtual void onControleDeattachment() { return; };

    public:
        bool isControlled() const;
        template<class T = Controller::Instance>
        T* getController() const
        {
            return dynamic_cast<T*>(m_controller);
        }
        void attachController(Controller::Instance* inController);
        void deattachController();

        CPhysics* getPhysicsComponent() const;

    protected:
        Controller::Instance*  m_controller;

        CPhysics*              m_physicsComponent;
    };
}