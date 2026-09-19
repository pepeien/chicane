#pragma once

#include "Chicane/Box.hpp"

namespace Chicane
{
    namespace Box
    {
        enum class FontVendor : std::uint8_t
        {
            Undefined, // N/A
            OpenType,  // .otf
            TrueType   // .ttf
        };
    }

    inline String toString(Box::FontVendor inValue)
    {
        switch (inValue)
        {
        case Box::FontVendor::Undefined:
            return "Undefined";

        case Box::FontVendor::OpenType:
            return "OpenType";

        case Box::FontVendor::TrueType:
            return "TrueType";

        default:
            return "";
        }
    }
}
