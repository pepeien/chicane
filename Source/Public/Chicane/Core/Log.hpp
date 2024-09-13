#pragma once

#include "Chicane/Base.hpp"

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

        void info(const std::string& inMessage);
        void warning(const std::string& inMessage);
        void error(const std::string& inMessage);
        void critical(const std::string& inMessage);
        void emmit(
            const std::string& inIdentifier,
            const std::string& inMessage,
            const std::string& inHexColor
        );
    }
}