#pragma once

#include "Chicane/Core/Essential.hpp"
#include "Chicane/Core/Color.hpp"
#include "Chicane/Core/Event.hpp"
#include "Chicane/Core/Log/Entry.hpp"
#include "Chicane/Core/String.hpp"

namespace Chicane
{
    namespace Log
    {
        using List             = std::deque<Entry>;
        using ListObservable   = Observable<List>;
        using ListSubscription = Subscription<List>;

        CHICANE_CORE ListSubscription* watchLogs(
            ListSubscription::NextCallback inNext,
            ListSubscription::ErrorCallback inError = nullptr,
            ListSubscription::CompleteCallback inComplete = nullptr
        );

        CHICANE_CORE void emmit(
            const std::string& inHexColor,
            const std::string& inIdentifier,
            const std::string& inMessage
        );
        template<typename... T>
        void emmit(
            const std::string& inHexColor,
            const std::string& inIdentifier,
            std::string inMessage,
            T... inParams
        )
        {
            emmit(
                inHexColor,
                inIdentifier,
                String::sprint(inMessage, inParams...)
            );
        }

        CHICANE_CORE void info(const std::string& inMessage);
        template<typename... T>
        void info(std::string inMessage, T... inParams)
        {
            info(String::sprint(inMessage, inParams...));
        }

        CHICANE_CORE void warning(const std::string& inMessage);
        template<typename... T>
        void warning(std::string inMessage, T... inParams)
        {
            warning(String::sprint(inMessage, inParams...));
        }

        CHICANE_CORE void error(const std::string& inMessage);
        template<typename... T>
        void error(std::string inMessage, T... inParams)
        {
            error(String::sprint(inMessage, inParams...));
        }

        CHICANE_CORE void critical(const std::string& inMessage);
        template<typename... T>
        void critical(std::string inMessage, T... inParams)
        {
            critical(String::sprint(inMessage, inParams...));
        }
    }
}