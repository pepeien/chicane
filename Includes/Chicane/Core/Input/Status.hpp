#pragma once

#include <cstdint>

#include "Chicane/Core.hpp"
#include "Chicane/Core/String.hpp"

namespace Chicane
{
    namespace Input
    {
        enum class Status : std::uint8_t
        {
            Pressed,
            Released
        };

        inline String toString(Status inValue)
        {
            switch (inValue)
            {
            case Status::Pressed:
                return "Pressed";

            case Status::Released:
                return "Released";

            default:
                return "";
            }
        }
    }
}