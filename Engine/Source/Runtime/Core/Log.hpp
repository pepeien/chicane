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