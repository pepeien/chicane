#pragma once

#include "Chicane/Base.hpp"
#include "Chicane/Core/Color.hpp"
#include "Chicane/Core/Event/Subscription.hpp"
#include "Chicane/Core/String.hpp"
#include "Chicane/Core/Log/Entry.hpp"

namespace Chicane
{
    namespace Log
    {
        typedef std::deque<Entry> List;

        Subscription<const List*>* watchLogs(
            std::function<void (const List*)> inNext,
            std::function<void (const std::string&)> inError = nullptr,
            std::function<void ()> inComplete = nullptr
        );

        void emmit(
            const std::string& inHexColor,
            const std::string& inIdentifier,
            const std::string& inMessage
        );
        template<typename... T>
        void emmit(
            const std::string& inHexColor,
            const std::string& inIdentifier,
            const std::string& inMessage,
            T... inParams
        )
        {
            emmit(
                inHexColor,
                inIdentifier,
                String::sprint(inMessage, inParams...)
            );
        }

        void info(const std::string& inMessage);
        template<typename... T>
        void info(const std::string& inMessage, T... inParams)
        {
            info(String::sprint(inMessage, inParams...));
        }

        void warning(const std::string& inMessage);
        template<typename... T>
        void warning(const std::string& inMessage, T... inParams)
        {
            warning(String::sprint(inMessage, inParams...));
        }

        void error(const std::string& inMessage);
        template<typename... T>
        void error(const std::string& inMessage, T... inParams)
        {
            error(String::sprint(inMessage, inParams...));
        }

        void critical(const std::string& inMessage);
        template<typename... T>
        void critical(const std::string& inMessage, T... inParams)
        {
            critical(String::sprint(inMessage, inParams...));
        }
    }
}