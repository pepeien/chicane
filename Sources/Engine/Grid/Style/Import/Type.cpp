#include "Chicane/Grid/Style/Import/Type.hpp"

namespace Chicane
{
    String toString(Grid::StyleImportType inValue)
    {
        switch (inValue)
        {
        case Grid::StyleImportType::Undefined:
            return "Undefined";

        case Grid::StyleImportType::Style:
            return "Style";

        case Grid::StyleImportType::Font:
            return "Font";

        case Grid::StyleImportType::Texture:
            return "Texture";

        default:
            return "";
        }
    }
}
