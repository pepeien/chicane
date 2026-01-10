#pragma once

#include "Chicane/Core.hpp"
#include "Chicane/Core/String.hpp"

namespace Chicane
{
    namespace Log
    {
        struct CHICANE_CORE Entry
        {
        public:
            Entry(const String& inText, const String& inColor)
                : text(inText),
                  color(inColor)
            {}

            Entry()
                : text(""),
                  color("")
            {}

        public:
            bool isEmpty() const { return text.isEmpty(); }

        public:
            String text;
            String color;
        };
    }
}