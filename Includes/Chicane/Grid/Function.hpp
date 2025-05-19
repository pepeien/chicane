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
            std::string            name   = "";
            std::vector<Reference> params = {};
        };

        typedef std::function<Reference (const Event&)> Function;
        typedef std::unordered_map<std::string, Function> Functions;
    }
}