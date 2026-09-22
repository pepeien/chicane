#include "Chicane/Kerb/Gravity/Planet.hpp"

namespace Chicane
{
    String toString(Kerb::Planet inValue)
    {
        switch (inValue)
        {
        case Kerb::Planet::Mercury:
            return "Mercury";

        case Kerb::Planet::Venus:
            return "Venus";

        case Kerb::Planet::Earth:
            return "Earth";

        case Kerb::Planet::Moon:
            return "Moon";

        case Kerb::Planet::Mars:
            return "Mars";

        case Kerb::Planet::Jupiter:
            return "Jupiter";

        case Kerb::Planet::Saturn:
            return "Saturn";

        case Kerb::Planet::Uranus:
            return "Uranus";

        case Kerb::Planet::Neptune:
            return "Neptune";

        case Kerb::Planet::Pluto:
            return "Pluto";

        default:
            return "";
        }
    }
}
