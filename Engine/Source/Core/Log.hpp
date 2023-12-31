#pragma once

#include "Base.hpp"

namespace Chicane
{
    namespace Log
    {
        enum class MessageType
        {
            INFO,
            WARNING,
            ERROR,
            CRITICAL
        };

        void info(const std::string& inMessage);
        void warning(const std::string& inMessage);
        void error(const std::string& inMessage);
        void critical(const std::string& inMessage);
        void emmit(MessageType inType, const std::string& inMessage);
    }
}

#ifndef NDEBUG
    #define LOG_INFO(message)     Chicane::Log::info(    message)
    #define LOG_WARNING(message)  Chicane::Log::warning( message)
    #define LOG_ERROR(message)    Chicane::Log::error(   message)
    #define LOG_CRITICAL(message) Chicane::Log::critical(message)
#else
    #define LOG_INFO(message)
    #define LOG_WARNING(message)
    #define LOG_ERROR(message)
    #define LOG_CRITICAL(message)
#endif