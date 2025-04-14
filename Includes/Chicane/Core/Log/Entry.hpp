#pragma once

#include "Chicane.hpp"

namespace Chicane
{
    namespace Log
    {
        struct Entry
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