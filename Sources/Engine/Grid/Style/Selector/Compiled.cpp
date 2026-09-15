#include "Chicane/Grid/Style/Selector/Compiled.hpp"

namespace Chicane
{
    namespace Grid
    {
        std::uint32_t StyleCompiledSelector::specificity() const
        {
            std::uint32_t result = 0U;

            for (const StyleSelectorPart& part : chain)
            {
                result += part.specificity();
            }

            return result;
        }
    }
}