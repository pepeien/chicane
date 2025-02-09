#include "Chicane/Core/String.hpp"

#include "Chicane/Core.hpp"

namespace Chicane
{
    namespace String
    {
        bool areEquals(const std::string& inA, const std::string& inB)
        {
            return strcmp(inA.c_str(), inB.c_str()) == 0;
        }

        std::string trim(const std::string& inTarget)
        {
            std::string result = inTarget;
            result.erase(result.find_last_not_of(' ') + 1);
            result.erase(0, result.find_first_not_of(' '));

            return result;
        }

        std::vector<std::string> split(const std::string& inTarget, char inDelimeter)
        {
            std::vector<std::string> result = {};

            if (inTarget.empty())
            {
                return result;
            }

            result.reserve(inTarget.size() / 2);

            std::string item = "";

            std::stringstream stream(inTarget);
            while (getline(stream, item, inDelimeter))
            {
                result.push_back(item);
            }

            return result;
        }

        std::vector<std::string> split(const std::vector<unsigned char>& inTarget, char inDelimiter)
        {
            return split(
                std::string(inTarget.begin(), inTarget.end()),
                inDelimiter
            );
        }

        void sort(std::vector<std::string>& outValue)
        {
            std::sort(
                outValue.begin(),
                outValue.end(),
                [](const std::string& inA, const std::string& inB)
                {
                    return strcmp(inA.c_str(), inB.c_str()) > 0;
                }
            );
        }
    }
}