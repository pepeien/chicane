#pragma once

#include "Chicane/Grid/Essential.hpp"
#include "Chicane/Grid/Event.hpp"

namespace Chicane
{
    namespace Grid
    {
        struct CHICANE_GRID FunctionData
        {
        public:
            String                 name   = "";
            std::vector<Reference> params = {};
        };

        using Function  = std::function<Reference (const Event&)>;
        using Functions = std::unordered_map<String, Function>;
    }
}