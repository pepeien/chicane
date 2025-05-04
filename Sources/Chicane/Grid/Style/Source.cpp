#include "Grid/Style/Source.hpp"

namespace Chicane
{
    namespace Grid
    {
        namespace Style
        {
            bool Source::isEmpty() const
            {
                return selector.empty() || properties.empty();
            }
        }
    }
}