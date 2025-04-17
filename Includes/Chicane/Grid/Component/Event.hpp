#pragma once

#include "Core.hpp"
#include "Grid/Reference.hpp"

namespace Chicane
{
    namespace Grid
    {
        namespace Component
        {
            struct CHICANE Event
            {
            public:
                typedef std::function<void (const Event&)> Function;

            public:
                std::vector<Reference> values = {};
            };
        }
    }
}