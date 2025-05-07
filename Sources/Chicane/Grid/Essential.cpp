#include "Grid/Essential.hpp"

namespace Chicane
{
    namespace Grid
    {
        std::string extractParams(const std::string& inValue)
        {
            const std::uint32_t start = inValue.find_first_of(FUNCTION_PARAMS_OPENING) + 1;
            const std::uint32_t end   = inValue.find_last_of(FUNCTION_PARAMS_CLOSING);

            return inValue.substr(start, end - start);
        }
    }
}