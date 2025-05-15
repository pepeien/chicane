#include "Chicane/Grid/Style/Essential.hpp"

#include "Chicane/Grid/Essential.hpp"

namespace Chicane
{
    namespace Grid
    {
        namespace Style
        {
            std::string variableToReference(const std::string& inValue)
            {
                const std::uint32_t start = inValue.find_first_of(FUNCTION_PARAMS_OPENING) + 1;
                const std::uint32_t end   = inValue.find_last_of(FUNCTION_PARAMS_CLOSING);

                std::string reference = "";
                reference.append(REFERENCE_VALUE_OPENING);
                reference.append(inValue.substr(start, end - start));
                reference.append(REFERENCE_VALUE_CLOSING);
                reference.append(inValue.substr(end + 1));

                return reference;
            }

            std::string colorToReference(const std::string& inValue)
            {
                if (String::startsWith(inValue, VARIABLE_KEYWORD))
                {
                    return variableToReference(inValue);
                }

                return inValue;
            }
        }
    }
}