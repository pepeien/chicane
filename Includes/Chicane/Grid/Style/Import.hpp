#pragma once

#include <vector>

#include "Chicane/Core/String.hpp"

#include "Chicane/Grid.hpp"
#include "Chicane/Grid/Style/Import/Type.hpp"
#include "Chicane/Grid/Style/Location.hpp"

namespace Chicane
{
    namespace Grid
    {
        /**
         * Style imports oughta be only used at the start of each file, following this template:
         *  `@import (StyleImportType, String);`
         */
        struct CHICANE_GRID StyleImport
        {
        public:
            using List = std::vector<StyleImport>;

        public:
            static constexpr inline const char* KEYWORD = "@import";

        public:
            StyleImport(const String& inValue);
            StyleImport();

        public:
            void parse(const String& inValue);

            void setType(const String& inValue);

            void setLocation(const String& inValue);

        public:
            StyleImportType type;
            StyleLocation   location;
        };
    }
}