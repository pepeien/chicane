#pragma once

#include "Chicane/Core.hpp"
#include "Chicane/Core/Reflection.hpp"
#include "Chicane/Core/String.hpp"

namespace Chicane
{
    namespace Log
    {
        CH_TYPE(Manual)
        struct CHICANE_CORE Entry
        {
        public:
            Entry(const String& inText, const String& inColor);
            Entry();

        public:
            CH_FUNCTION()
            bool isEmpty() const;

        public:
            CH_FIELD()
            String text;
            CH_FIELD()
            String color;
        };
    }
}