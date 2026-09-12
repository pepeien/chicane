#pragma once

#include <cstdint>

#include "Chicane/Drift.hpp"
#include "Chicane/Core/String.hpp"

namespace Chicane
{
    namespace Drift
    {
        enum class Status : std::uint8_t
        {
            Stopped,
            Playing,
            Paused
        };

        inline String toString(Status inValue)
        {
            switch (inValue)
            {
            case Status::Stopped:
                return "Stopped";

            case Status::Playing:
                return "Playing";

            case Status::Paused:
                return "Paused";

            default:
                return "";
            }
        }
    }
}
