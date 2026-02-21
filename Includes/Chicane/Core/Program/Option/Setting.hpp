#pragma once

#include <vector>

#include "Chicane/Core.hpp"
#include "Chicane/Core/String.hpp"

namespace Chicane
{
    class CHICANE_CORE ProgramOptionSetting
    {
    public:
        bool                bIsRequired = false;
        String              name        = "";
        String              description = "";
        std::vector<String> validValues = {};
    };
}