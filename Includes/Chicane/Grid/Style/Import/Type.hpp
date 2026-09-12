#pragma once

#include <cstdint>

#include "Chicane/Grid.hpp"
#include "Chicane/Core/String.hpp"

namespace Chicane
{
    namespace Grid
    {
        enum class StyleImportType : std::uint8_t
        {
            Undefined,
            Style,
            Font,
            Texture
        };

        inline String toString(StyleImportType inValue)
        {
            switch (inValue)
            {
            case StyleImportType::Undefined:
                return "Undefined";

            case StyleImportType::Style:
                return "Style";

            case StyleImportType::Font:
                return "Font";

            case StyleImportType::Texture:
                return "Texture";

            default:
                return "";
            }
        }
    }
}