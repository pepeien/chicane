#include "Chicane/Grid/Style/Source.hpp"

namespace Chicane
{
    namespace Grid
    {
        namespace Style
        {
            Source::Source(const std::vector<std::string>& inSelectors, const Properties& inProperties)
                : selectors(inSelectors),
                properties(inProperties)
            {}

            Source::Source()
                : selectors({}),
                properties({})
            {}

            bool Source::isEmpty() const
            {
                return selectors.empty() || properties.empty();
            }
        }
    }
}