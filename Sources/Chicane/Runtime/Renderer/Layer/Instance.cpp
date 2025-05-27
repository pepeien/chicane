#include "Chicane/Runtime/Renderer/Layer/Instance.hpp"

namespace Chicane
{
    namespace Layer
    {
        Instance::Instance()
            : Instance("Unamed")
        {}

        Instance::Instance(const std::string& inId)
            : m_id(inId),
              m_status(Status::Offline)
        {}

        void Instance::init()
        {
            if (!is(Status::Offline))
            {
                return;
            }

            if (!onInit())
            {
                return;
            }

            setStatus(Status::Initialized);
        }

        void Instance::destroy()
        {
            if (is(Status::Offline))
            {
                return;
            }
    
            if (!onDestroy())
            {
                return;
            }

            setStatus(Status::Offline);
        }

        void Instance::rebuild()
        {
            if (!is(Status::Offline))
            {
                return;
            }

            if (!onRebuild())
            {
                return;
            }

            setStatus(Status::Initialized);
        }

        void Instance::setup()
        {
            if (is(Status::Offline))
            {
                return;
            }

            if (!onSetup())
            {
                return;
            }

            setStatus(Status::Running);
        }

        void Instance::render(void* outData)
        {
            if (!is(Status::Running))
            {
                return;
            }

            onRender(outData);
        }

        void Instance::handle(const Window::Event& inEvent)
        {
            onEvent(inEvent);
        }

        bool Instance::is(Status inStatus) const
        {
            return m_status == inStatus;
        }

        Status Instance::getStatus() const
        {
            return m_status;
        }

        const std::string& Instance::getId() const
        { 
            return m_id;
        }

        void Instance::setStatus(Status inStatus)
        {
            m_status = inStatus;
        }
    }
}
