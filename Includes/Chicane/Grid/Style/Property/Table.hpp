#pragma once

#include <cstddef>

#include "Chicane/Core/String.hpp"

#include "Chicane/Grid.hpp"
#include "Chicane/Grid/Style/Property/Entry.hpp"
#include "Chicane/Grid/Style/Property/Id.hpp"

namespace Chicane
{
    namespace Grid
    {
        struct CHICANE_GRID StylePropertyTable
        {
        public:
            static constexpr inline const std::size_t COUNT = static_cast<std::size_t>(StylePropertyId::Count);

            static constexpr inline const std::size_t VALUE_COUNT = 64;

        public:
            static const StylePropertyEntry& sGet(StylePropertyId inId);

            static bool sFind(const String& inName, StylePropertyId& outId);

            static std::size_t sOffset(StylePropertyId inId);
        };
    }
}
