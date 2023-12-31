#pragma once

#include "Base.hpp"

namespace Chicane
{
    namespace Helper
    {
        std::vector<std::string> splitString(
            const std::vector<char>& inTarget,
            const std::string& inDelimiter
        );
        std::vector<std::string> splitString(
            const std::string& inTarget,
            const std::string& inDelimiter
        );
    }
}