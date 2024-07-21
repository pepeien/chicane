#pragma once

#include <string>
#include <vector>

namespace Chicane
{
    namespace Utils
    {
        std::string trim(const std::string& inTarget);
        std::vector<std::string> split(const std::string& inTarget, char inDelimeter);
    }
}