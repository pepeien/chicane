#include "Chicane/Grid/Style/Essential.hpp"

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

            std::vector<std::string> splitOneliner(const std::string& inValue)
            {
                std::vector<std::string> result = {};

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

                    std::string block = inValue.substr(start, end - start);

                    start = end;

                    if (block.empty())
                    {
                        continue;
                    }

                    result.push_back(String::trim(block));
                }

                result.push_back(inValue.substr(start, end - start));

                return result;
            }
        }
    }
}