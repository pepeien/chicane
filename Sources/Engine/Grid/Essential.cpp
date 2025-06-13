#include "Chicane/Grid/Essential.hpp"

namespace Chicane
{
    namespace Grid
    {
        String extractParams(const String& inValue)
        {
            const std::uint32_t start = inValue.firstOf(FUNCTION_PARAMS_OPENING) + 1;
            const std::uint32_t end   = inValue.lastOf(FUNCTION_PARAMS_CLOSING);

            return inValue.substr(start, end - start);
        }
    }
}