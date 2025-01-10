#pragma once

#include "Chicane/Base.hpp"

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
                std::string           name   = "";
                std::vector<std::any> params = {};

            public:
                static const FunctionData& empty();
            };

            struct Event
            {
            public:
                std::vector<std::any> values = {};
            };

            typedef std::function<std::any (const Event&)> Function;
            typedef std::unordered_map<std::string, Function> Functions;
        }
    }
}