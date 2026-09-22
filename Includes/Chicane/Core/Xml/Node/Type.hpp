#pragma once

#include <cstdint>

#include "Chicane/Core.hpp"
#include "Chicane/Core/Reflection.hpp"
#include "Chicane/Core/String.hpp"

namespace Chicane
{
    CH_ENUM()
    enum class XmlNodeType : std::uint8_t
    {
        Null,
        Element,
        Pcdata,
        Cdata,
        Comment,
        Other
    };

    CHICANE_CORE String toString(XmlNodeType inValue);
}
