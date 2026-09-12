#pragma once

#include <cstdint>

#include "Chicane/Core/String.hpp"

namespace Chicane
{
    namespace Kerb
    {
        enum class Planet : std::uint8_t
        {
            Mercury,
            Venus,
            Earth,
            Moon,
            Mars,
            Jupiter,
            Saturn,
            Uranus,
            Neptune,
            Pluto
        };

        inline String toString(Planet inValue)
        {
            switch (inValue)
            {
            case Planet::Mercury:
                return "Mercury";

            case Planet::Venus:
                return "Venus";

            case Planet::Earth:
                return "Earth";

            case Planet::Moon:
                return "Moon";

            case Planet::Mars:
                return "Mars";

            case Planet::Jupiter:
                return "Jupiter";

            case Planet::Saturn:
                return "Saturn";

            case Planet::Uranus:
                return "Uranus";

            case Planet::Neptune:
                return "Neptune";

            case Planet::Pluto:
                return "Pluto";

            default:
                return "";
            }
        }
    }
}
