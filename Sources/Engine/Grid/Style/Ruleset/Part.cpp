#include "Chicane/Grid/Style/Ruleset/Part.hpp"

namespace Chicane
{
    namespace Grid
    {
        std::uint32_t StyleSelectorPart::specificity() const
        {
            std::uint32_t result = static_cast<std::uint32_t>(classes.size()) * 10U;

            if (!id.isEmpty())
            {
                result += 100U;
            }

            if (!tag.isEmpty())
            {
                result += 1U;
            }

            for (std::uint8_t flags = static_cast<std::uint8_t>(status); flags != 0;
                 flags              = static_cast<std::uint8_t>(flags & static_cast<std::uint8_t>(flags - 1U)))
            {
                result += 10U;
            }

            return result;
        }
    }
}