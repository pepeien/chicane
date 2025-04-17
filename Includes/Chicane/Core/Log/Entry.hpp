#pragma once

#include "Core/Essentials.hpp"

namespace Chicane
{
    namespace Log
    {
        struct CHICANE Entry
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