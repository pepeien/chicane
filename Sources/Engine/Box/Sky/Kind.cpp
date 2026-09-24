#include "Chicane/Box/Sky/Kind.hpp"

namespace Chicane
{
    String toString(Box::SkyKind inValue)
    {
        switch (inValue)
        {
        case Box::SkyKind::Panorama:
            return "Panorama";

        default:
            return "Cube";
        }
    }
}
