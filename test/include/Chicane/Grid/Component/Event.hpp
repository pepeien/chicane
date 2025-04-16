#pragma once

#include "Chicane/Core.hpp"
#include "Chicane/Grid/Reference.hpp"

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