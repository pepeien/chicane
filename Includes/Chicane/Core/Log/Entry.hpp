#pragma once

#include "Chicane/Core/Essential.hpp"

namespace Chicane
{
    namespace Log
    {
        struct CHICANE_CORE Entry
        {
        public:
            Entry(const std::string& inText, const std::string& inColor)
                : text(inText),
                color(inColor)
            {}

            Entry()
                : text(""),
                color("")
            {}

        public:
            bool isEmpty() const
            {
                return text.empty();
            }

        public:
            std::string text;
            std::string color;
        };
    }
}