#pragma once

#include "Chicane/Base.hpp"
#include "Chicane/Core/Event.hpp"

namespace Chicane
{
    class Manager
    {
    public:
        enum class EventType : std::uint8_t
        {
            Load,
            Allocation,
            Use
        };
    
    public:
        Manager();
    
    public:
        void watchChanges(
            std::function<void (EventType)> inNextCallback,
            std::function<void (const std::string&)> inErrorCallback = nullptr,
            std::function<void ()> inCompleteCallback = nullptr
        );

    protected:
        std::unique_ptr<Observable<EventType>> m_observable;
    };
}