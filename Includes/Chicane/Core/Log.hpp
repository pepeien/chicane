#pragma once

#include <deque>

#include "Chicane/Core.hpp"
#include "Chicane/Core/Color.hpp"
#include "Chicane/Core/Event/Observable.hpp"
#include "Chicane/Core/Event/Subscription.hpp"
#include "Chicane/Core/Log/Entry.hpp"
#include "Chicane/Core/String.hpp"

namespace Chicane
{
    namespace Log
    {
        using List             = std::deque<Entry>;
        using ListObservable   = EventObservable<List>;
        using ListSubscription = EventSubscription<List>;

        CHICANE_CORE ListSubscription watchLogs(
            ListSubscription::NextCallback     inNext,
            ListSubscription::ErrorCallback    inError    = nullptr,
            ListSubscription::CompleteCallback inComplete = nullptr
        );

        CHICANE_CORE void emmit(
            const String& inHexColor, const String& inIdentifier, const String& inMessage
        );
        template <typename... T>
        void emmit(
            const String& inHexColor, const String& inIdentifier, String inMessage, T... inParams
        )
        {
            emmit(inHexColor, inIdentifier, String::sprint(inMessage, inParams...));
        }

        CHICANE_CORE void info(const String& inMessage);
        template <typename... T>
        void info(String inMessage, T... inParams)
        {
            info(String::sprint(inMessage, inParams...));
        }

        CHICANE_CORE void warning(const String& inMessage);
        template <typename... T>
        void warning(String inMessage, T... inParams)
        {
            warning(String::sprint(inMessage, inParams...));
        }

        CHICANE_CORE void error(const String& inMessage);
        template <typename... T>
        void error(String inMessage, T... inParams)
        {
            error(String::sprint(inMessage, inParams...));
        }

        CHICANE_CORE void critical(const String& inMessage);
        template <typename... T>
        void critical(String inMessage, T... inParams)
        {
            critical(String::sprint(inMessage, inParams...));
        }
    }
}