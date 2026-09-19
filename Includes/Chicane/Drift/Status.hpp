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
    }

    inline String toString(Drift::Status inValue)
    {
        switch (inValue)
        {
        case Drift::Status::Stopped:
            return "Stopped";

        case Drift::Status::Playing:
            return "Playing";

        case Drift::Status::Paused:
            return "Paused";

        default:
            return "";
        }
    }
}
