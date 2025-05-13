#include "Chicane/Grid/Style/Source.hpp"

namespace Chicane
{
    namespace Grid
    {
        namespace Style
        {
            bool Source::isEmpty() const
            {
                return selectors.empty() || properties.empty();
            }
        }
    }
}