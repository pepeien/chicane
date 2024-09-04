#pragma once

#include "Chicane/Base.hpp"

namespace Chicane
{
    namespace Log
    {
        enum class Color : std::uint8_t
        {
            Red,
            Blue,
            Green,
            Yellow,
            Orange,
            Cyan,
            White
        };

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
            Color inColor
        );
    }
}

#define LOG_INFO(    message) Chicane::Log::info(    message)
#define LOG_WARNING( message) Chicane::Log::warning( message)
#define LOG_ERROR(   message) Chicane::Log::error(   message)
#define LOG_CRITICAL(message) Chicane::Log::critical(message)

#define LOG_EMMIT(identifier, message, color) Chicane::Log::emmit(identifier, message, color)