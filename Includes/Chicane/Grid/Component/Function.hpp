#pragma once

#include "Core.hpp"
#include "Grid/Component/Event.hpp"
#include "Grid/Reference.hpp"

namespace Chicane
{
    namespace Grid
    {
        namespace Component
        {
            struct CHICANE FunctionData
            {
            public:
                std::string            name   = "";
                std::vector<Reference> params = {};

            public:
                static const FunctionData& empty();
            };

            typedef std::function<Reference (const Event&)> Function;
            typedef std::unordered_map<std::string, Function> Functions;
        }
    }
}