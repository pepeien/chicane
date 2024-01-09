#pragma once

#include "Base.hpp"

namespace Engine
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
    #define LOG_INFO(message)     Engine::Log::info(    message)
    #define LOG_WARNING(message)  Engine::Log::warning( message)
    #define LOG_ERROR(message)    Engine::Log::error(   message)
    #define LOG_CRITICAL(message) Engine::Log::critical(message)
#else
    #define LOG_INFO(message)
    #define LOG_WARNING(message)
    #define LOG_ERROR(message)
    #define LOG_CRITICAL(message)
#endif