#pragma once

#include <cstdint>

#include "Chicane/Grid.hpp"
#include "Chicane/Grid/Style/Property/Dirty.hpp"

namespace Chicane
{
    namespace Grid
    {
        struct Style;

        struct CHICANE_GRID StylePropertyEntry
        {
        public:
            // `read` reports whether the property currently takes part in animations
            // `auto` sizes and unset origins
            using Read  = bool (*)(const Style& inStyle, float* outValues);
            using Write = void (*)(Style& outStyle, const float* inValues);

        public:
            const char*        name  = "";
            std::uint8_t       arity = 1;
            StylePropertyDirty dirty = StylePropertyDirty::None;
            Read               read  = nullptr;
            Write              write = nullptr;
        };
    }
}
