#pragma once

#include "Chicane/Core.hpp"
#include "Chicane/Grid/Event.hpp"
#include "Chicane/Grid/Reference.hpp"

namespace Chicane
{
    namespace Grid
    {
        struct CHICANE FunctionData
        {
        public:
            std::string            name   = "";
            std::vector<Reference> params = {};
        };

        typedef std::function<Reference (const Event&)> Function;
        typedef std::unordered_map<std::string, Function> Functions;
    }
}