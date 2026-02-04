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
        using List = std::deque<Entry>;

        CHICANE_CORE const List& getLogs();

        CHICANE_CORE void emmit(const String& inHexColor, const String& inIdentifier, const String& inMessage);
        template <typename... T>
        inline void emmit(const String& inHexColor, const String& inIdentifier, String inMessage, T... inParams)
        {
            emmit(inHexColor, inIdentifier, String::sprint(inMessage, inParams...));
        }

        CHICANE_CORE void info(const String& inMessage);
        template <typename... T>
        inline void info(String inMessage, T... inParams)
        {
            info(String::sprint(inMessage, inParams...));
        }

        CHICANE_CORE void warning(const String& inMessage);
        template <typename... T>
        inline void warning(String inMessage, T... inParams)
        {
            warning(String::sprint(inMessage, inParams...));
        }

        CHICANE_CORE void error(const String& inMessage);
        template <typename... T>
        inline void error(String inMessage, T... inParams)
        {
            error(String::sprint(inMessage, inParams...));
        }

        CHICANE_CORE void critical(const String& inMessage);
        template <typename... T>
        inline void critical(String inMessage, T... inParams)
        {
            critical(String::sprint(inMessage, inParams...));
        }
    }
}