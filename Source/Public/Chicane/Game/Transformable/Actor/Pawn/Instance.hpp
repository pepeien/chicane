#pragma once

#include "Chicane/Base.hpp"
#include "Chicane/Game/Transformable/Actor/Instance.hpp"

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
        template<class T = Controller::Instance>
        T* getController() const
        {
            return dynamic_cast<T*>(m_controller);
        }

        bool isControlled() const;
        void attachController(Controller::Instance* inController);
        void deattachController();

    protected:
        Controller::Instance*  m_controller;
    };
}