#pragma once

#include "Chicane/Base.hpp"
#include "Chicane/Grid/Reference.hpp"

namespace Chicane
{
    namespace Grid
    {
        namespace Component
        {
            typedef std::function<void (const pugi::xml_node&)> Compiler;

            struct FunctionData
            {
            public:
                std::string            name   = "";
                std::vector<Reference> params = {};

            public:
                static const FunctionData& empty();
            };

            struct Event
            {
            public:
                std::vector<Reference> values = {};
            };

            typedef std::function<Reference (const Event&)> Function;
            typedef std::function<void (const Event&)> EventFunction;
            typedef std::unordered_map<std::string, Function> Functions;
        }
    }
}