#include "Chicane/Grid/Style.hpp"

namespace Chicane
{
    namespace Grid
    {
        namespace Style
        {
            String variableToReference(const String& inValue)
            {
                const std::uint32_t start = inValue.firstOf(FUNCTION_PARAMS_OPENING) + 1;
                const std::uint32_t end   = inValue.lastOf(FUNCTION_PARAMS_CLOSING);

                String reference = "";
                reference.append(REFERENCE_VALUE_OPENING);
                reference.append(inValue.substr(start, end - start));
                reference.append(REFERENCE_VALUE_CLOSING);
                reference.append(inValue.substr(end + 1));

                return reference;
            }

            String colorToReference(const String& inValue)
            {
                if (inValue.startsWith(VARIABLE_KEYWORD))
                {
                    return variableToReference(inValue);
                }

                return inValue;
            }

            std::vector<String> splitOneliner(const String& inValue)
            {
                std::vector<String> result = {};

                std::uint32_t start = 0;
                std::uint32_t end   = 0;

                std::uint32_t parathesisCount = 0;

                for (std::uint32_t i = 0; i < inValue.size(); i++)
                {
                    const char character = inValue.at(i);

                    end++;

                    if (character == FUNCTION_PARAMS_OPENING)
                    {
                        parathesisCount++;

                        continue;
                    }

                    if (character == FUNCTION_PARAMS_CLOSING)
                    {
                        parathesisCount--;

                        continue;
                    }

                    if (character != ONELINE_SEPARATOR || parathesisCount > 0)
                    {
                        continue;
                    }

                    const String block = inValue.substr(start, end - start);

                    start = end;

                    if (block.isEmpty())
                    {
                        continue;
                    }

                    result.push_back(block.trim());
                }

                result.push_back(inValue.substr(start, end - start));

                return result;
            }
        }
    }
}