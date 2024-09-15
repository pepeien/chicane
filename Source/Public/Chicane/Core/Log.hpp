#pragma once

#include "Chicane/Base.hpp"
#include "Chicane/Core/Utils.hpp"

namespace Chicane
{
    namespace Log
    {
        constexpr auto COLOR_RED          = "#F73B3B";
        constexpr auto COLOR_GREEN        = "#1FED4F";
        constexpr auto COLOR_LIGHT_GREEN  = "#07e8b7";
        constexpr auto COLOR_BLUE         = "#4E8DDE";
        constexpr auto COLOR_YELLOW       = "#E8E805";
        constexpr auto COLOR_ORANGE       = "#fc850d";
        constexpr auto COLOR_CYAN         = "#05AEB0";
        constexpr auto COLOR_WHITE        = "#FFFFFF";

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

        typedef std::vector<Instance> List;

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