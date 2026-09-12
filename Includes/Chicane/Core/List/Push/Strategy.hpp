#pragma once

#include <cstdint>

#include "Chicane/Core.hpp"
#include "Chicane/Core/String.hpp"

namespace Chicane
{
    enum class ListPushStrategy : std::uint8_t
    {
        Front,
        Back,
        Before,
        After
    };

    inline String toString(ListPushStrategy inValue)
    {
        switch (inValue)
        {
        case ListPushStrategy::Front:
            return "Front";

        case ListPushStrategy::Back:
            return "Back";

        case ListPushStrategy::Before:
            return "Before";

        case ListPushStrategy::After:
            return "After";

        default:
            return "";
        }
    }
}