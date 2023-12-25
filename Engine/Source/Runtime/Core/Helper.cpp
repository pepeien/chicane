#include "Helper.hpp"

namespace Chicane
{
    namespace Helper
    {
        std::vector<std::string> splitString(
            const std::vector<char>& inTarget,
            const std::string& inDelimiter
        )
        {
            return splitString(
                std::string(inTarget.begin(), inTarget.end()),
                inDelimiter
            );
        }

        std::vector<std::string> splitString(
            const std::string& inTarget,
            const std::string& inDelimiter
        )
        {
            size_t startPosition   = 0;
            size_t endPosition     = 0;
            size_t delimiterLength = inDelimiter.size();

            std::string token;
            std::vector<std::string> res;

            while ((endPosition = inTarget.find(inDelimiter, startPosition)) != std::string::npos)
            {
                token         = inTarget.substr(startPosition, endPosition - startPosition);
                startPosition = endPosition + delimiterLength;

                res.push_back(token);
            }

            res.push_back(inTarget.substr(startPosition));

            return res;
        }
    }
}