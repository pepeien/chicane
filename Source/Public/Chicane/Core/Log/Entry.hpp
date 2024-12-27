#pragma once

#include "Chicane/Base.hpp"

namespace Chicane
{
    namespace Log
    {
        struct Entry
        {
            std::string text  = "";
            std::string color = "";

        public:
            bool isEmpty() const
            {
                return text.empty();
            }
        };
    }
}