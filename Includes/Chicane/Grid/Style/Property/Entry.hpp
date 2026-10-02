#pragma once

#include <cstdint>
#include <functional>

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
            using Read  = std::function<bool(const Style& inStyle, float* outValues)>;
            using Write = std::function<void(Style& outStyle, const float* inValues)>;

        public:
            const char*        name  = "";
            std::uint8_t       arity = 1;
            StylePropertyDirty dirty = StylePropertyDirty::None;
            Read               read  = nullptr;
            Write              write = nullptr;
        };
    }
}
