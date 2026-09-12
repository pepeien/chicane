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

        inline String toString(FontVendor inValue)
        {
            switch (inValue)
            {
            case FontVendor::Undefined:
                return "Undefined";

            case FontVendor::OpenType:
                return "OpenType";

            case FontVendor::TrueType:
                return "TrueType";

            default:
                return "";
            }
        }
    }
}