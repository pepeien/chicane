#pragma once

#include "Chicane/Core/Essential.hpp"

namespace Chicane
{
    namespace Log
    {
        struct CHICANE_CORE Entry
        {
        public:
            bool isEmpty() const
            {
                return text.empty();
            }

        public:
            std::string text  = "";
            std::string color = "";
        };
    }
}