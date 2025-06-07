#pragma once

#include "Chicane/Grid/Style/Alignment.hpp"
#include "Chicane/Grid/Style/Corners.hpp"
#include "Chicane/Grid/Style/Constants.hpp"
#include "Chicane/Grid/Style/Direction.hpp"
#include "Chicane/Grid/Style/Display.hpp"
#include "Chicane/Grid/Style/Essential.hpp"
#include "Chicane/Grid/Style/Flex.hpp"
#include "Chicane/Grid/Style/Font.hpp"
#include "Chicane/Grid/Style/Instance.hpp"
#include "Chicane/Grid/Style/Position.hpp"
#include "Chicane/Grid/Style/Source.hpp"

namespace Chicane
{
    namespace Grid
    {
        namespace Style
        {
            CHICANE_GRID std::string variableToReference(const std::string& inValue);
            CHICANE_GRID std::string colorToReference(const std::string& inValue);

            CHICANE_GRID std::vector<std::string> splitOneliner(const std::string& inValue);
        }
    }
}