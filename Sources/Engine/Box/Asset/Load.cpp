#include "Chicane/Box/Asset/Load.hpp"

namespace Chicane
{
    String toString(Box::AssetLoad inValue)
    {
        switch (inValue)
        {
        case Box::AssetLoad::Header:
            return "Header";

        case Box::AssetLoad::Preview:
            return "Preview";

        case Box::AssetLoad::Full:
            return "Full";

        default:
            return "";
        }
    }
}
