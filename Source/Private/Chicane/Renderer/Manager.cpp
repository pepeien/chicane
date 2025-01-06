#include "Chicane/Renderer/Manager.hpp"

namespace Chicane
{
    Manager::Manager()
        : m_observable(std::make_unique<Observable<Chicane::Manager::EventType>>())
    {}

    void Manager::watchChanges(
        std::function<void (Chicane::Manager::EventType)> inNextCallback,
        std::function<void (const std::string&)> inErrorCallback,
        std::function<void ()> inCompleteCallback
    )
    {
        m_observable->subscribe(
            inNextCallback,
            inErrorCallback,
            inCompleteCallback
        );
    }
}