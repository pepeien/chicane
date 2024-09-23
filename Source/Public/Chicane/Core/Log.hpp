#pragma once

#include "Chicane/Base.hpp"
#include "Chicane/Core/Color.hpp"
#include "Chicane/Core/Utils.hpp"

namespace Chicane
{
    namespace Log
    {
        struct Instance
        {
            std::string text  = "";
            std::string color = "";

        public:
            bool isEmpty() const
            {
                return text.empty();
            }
        };

        typedef std::deque<Instance> List;

        void watchLogs(
            std::function<void (const List&)> inNextCallback,
            std::function<void (const std::string&)> inErrorCallback = nullptr,
            std::function<void ()> inCompleteCallback = nullptr
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
                Utils::sprint(inMessage, inParams...)
            );
        }

        void info(const std::string& inMessage);
        template<typename... T>
        void info(const std::string& inMessage, T... inParams)
        {
            info(Utils::sprint(inMessage, inParams...));
        }

        void warning(const std::string& inMessage);
        template<typename... T>
        void warning(const std::string& inMessage, T... inParams)
        {
            warning(Utils::sprint(inMessage, inParams...));
        }

        void error(const std::string& inMessage);
        template<typename... T>
        void error(const std::string& inMessage, T... inParams)
        {
            error(Utils::sprint(inMessage, inParams...));
        }

        void critical(const std::string& inMessage);
        template<typename... T>
        void critical(const std::string& inMessage, T... inParams)
        {
            critical(Utils::sprint(inMessage, inParams...));
        }
    }
}