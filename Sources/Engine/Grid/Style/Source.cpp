#include "Chicane/Grid/Style/Source.hpp"

namespace Chicane
{
    namespace Grid
    {
        StyleSource::StyleSource(
            const std::vector<String>& inSelectors, const Map& inProperties
        )
            : selectors(inSelectors),
              properties(inProperties)
        {}

        StyleSource::StyleSource()
            : selectors({}),
              properties({})
        {}

        bool StyleSource::isEmpty() const
        {
            return selectors.empty() || properties.empty();
        }
    }
}