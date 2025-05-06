#include "Core/String.hpp"

namespace Chicane
{
    namespace String
    {
        bool areEquals(const std::string& inA, const std::string& inB)
        {
            return strcmp(inA.c_str(), inB.c_str()) == 0;
        }

        bool contains(const std::string& inTarget, const std::string& inValue)
        {
            return inTarget.find(inValue) != std::string::npos;
        }

        bool startsWith(const std::string& inTarget, const std::string& inValue)
        {
            if (inTarget.empty() || inTarget.size() < inValue.size())
            {
                return false;
            }

            return String::areEquals(inTarget.substr(0, inValue.size()), inValue);
        }

        bool endsWith(const std::string& inTarget, const std::string& inValue)
        {
            if (inTarget.empty() || inTarget.size() < inValue.size())
            {
                return false;
            }

            return String::areEquals(
                inTarget.substr(inTarget.size() - inValue.size()),
                inValue
            );
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
                result.push_back(String::trim(item));
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

        std::string join(
            const std::vector<std::string>& inTarget,
            const std::string& inJoiner,
            std::uint32_t inStart,
            std::uint32_t inEnd
        )
        {
            const std::uint32_t size  = static_cast<std::uint32_t>(inTarget.size() - 1);
            const std::uint32_t start = std::max(
                static_cast<std::uint32_t>(0),
                std::min(inStart, size)
            );
            const std::uint32_t end = std::max(
                start,
                inStart == 0 && inEnd == 0 ? size : std::min(inEnd, size)
            );

            std::string result = "";

            for (std::uint32_t i = start; i <= end; i++)
            {
                result.append(inTarget.at(i));
                result.append(inJoiner);
            }

            return result;
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

        bool toBool(const std::string& inTarget)
        {
            if (inTarget.empty()) {
                return false;
            }

            return areEquals(inTarget, "1") || areEquals(inTarget, "true");
        }
    }
}