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

    inline String toString(XmlNodeType inValue)
    {
        switch (inValue)
        {
        case XmlNodeType::Null:
            return "Null";

        case XmlNodeType::Element:
            return "Element";

        case XmlNodeType::Pcdata:
            return "Pcdata";

        case XmlNodeType::Cdata:
            return "Cdata";

        case XmlNodeType::Comment:
            return "Comment";

        case XmlNodeType::Other:
            return "Other";

        default:
            return "";
        }
    }
}
