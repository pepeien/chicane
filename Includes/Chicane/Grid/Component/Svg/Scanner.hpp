#pragma once

#include "Chicane/Core/String.hpp"

#include "Chicane/Grid.hpp"

namespace Chicane
{
    namespace Grid
    {
        struct CHICANE_GRID SvgScanner
        {
        public:
            SvgScanner();
            SvgScanner(const String& inValue);

        public:
            void skip();
            bool done();
            bool hasNumber();
            float number();
            bool flag();
            char command();

        public:
            const char* p;
            const char* end;
        };
    }
}
