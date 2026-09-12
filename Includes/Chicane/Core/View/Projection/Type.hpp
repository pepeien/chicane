#pragma once

#include <cstdint>

#include "Chicane/Core.hpp"
#include "Chicane/Core/String.hpp"

namespace Chicane
{
    enum class ViewProjectionType : std::uint8_t
    {
        Orthographic,
        Perspective
    };

    inline String toString(ViewProjectionType inValue)
    {
        switch (inValue)
        {
        case ViewProjectionType::Orthographic:
            return "Orthographic";

        case ViewProjectionType::Perspective:
            return "Perspective";

        default:
            return "";
        }
    }
}