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

        void info(const std::string& inMessage);
        void warning(const std::string& inMessage);
        void error(const std::string& inMessage);
        void critical(const std::string& inMessage);
        void emmit(
            Color inColor,
            const std::string& inMessage
        );
    }
}

#ifdef IS_DEBUGGING
    #define LOG_INFO(    message) Chicane::Log::info(    message)
    #define LOG_WARNING( message) Chicane::Log::warning( message)
    #define LOG_ERROR(   message) Chicane::Log::error(   message)
    #define LOG_CRITICAL(message) Chicane::Log::critical(message)

    #define LOG_EMMIT(color, message) Chicane::Log::emmit(color, message)
#else
    #define LOG_INFO(message)
    #define LOG_WARNING(message)
    #define LOG_ERROR(message)
    #define LOG_CRITICAL(message)
    #define LOG_EMMIT(color, message)
#endif