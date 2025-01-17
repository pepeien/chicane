#pragma once

#include "Chicane/Base.hpp"
#include "Chicane/Grid/Reference.hpp"

namespace Chicane
{
    namespace Grid
    {
        namespace Component
        {
            struct Event
            {
            public:
                typedef std::function<void (const Event&)> Function;

            public:
                std::vector<Reference> values = {};
            };
        }
    }
}