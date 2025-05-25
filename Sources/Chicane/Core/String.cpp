#include "Chicane/Core/String.hpp"

#include "Chicane/Core/Log.hpp"

namespace Chicane
{
    namespace String
    {
        bool areEquals(const std::string& inA, const std::string& inB)
        {
            return strcmp(inA.c_str(), inB.c_str()) == 0;
        }

        bool areEquals(const std::string& inA, char inB)
        {
            return strcmp(inA.c_str(), std::string(1, inB).c_str()) == 0;
        }

        bool contains(const std::string& inTarget, const std::string& inValue)
        {
            return inTarget.find(inValue) != std::string::npos;
        }

        bool contains(const std::string& inTarget, char inValue)
        {
            return inTarget.find(inValue) != std::string::npos;
        }

        bool startsWith(const std::string& inTarget, const std::string& inValue)
        {
            if (inTarget.empty() || inTarget.size() < inValue.size())
            {
                return false;
            }

            return areEquals(inTarget.substr(0, inValue.size()), inValue);
        }

        bool startsWith(const std::string& inTarget, char inValue)
        {
            if (inTarget.empty())
            {
                return false;
            }

            return areEquals(inTarget.substr(0, 1), inValue);
        }

        bool endsWith(const std::string& inTarget, const std::string& inValue)
        {
            if (inTarget.empty() || inTarget.size() < inValue.size())
            {
                return false;
            }

            return areEquals(
                inTarget.substr(inTarget.size() - inValue.size()),
                inValue
            );
        }

        std::string trim(const std::string& inValue)
        {
            std::string result(inValue);
            result.erase(0, result.find_first_not_of(" \n\r\t"));
            result.erase(result.find_last_not_of(" \n\r\t") + 1);

            return result;
        }

        std::string toLower(const std::string& inValue)
        {
            std::string value = inValue;
            std::transform(
                value.begin(),
                value.end(),
                value.begin(),
                ::tolower
            );

            return value;
        }

        std::string toUpper(const std::string& inValue)
        {
            std::string value = inValue;
            std::transform(
                value.begin(),
                value.end(),
                value.begin(),
                ::toupper
            );

            return value;
        }

        std::vector<std::string> split(
            const std::vector<unsigned char>& inValue,
            const std::string& inDelimiter
        )
        {
            return split(
                std::string(inValue.begin(), inValue.end()),
                inDelimiter
            );
        }

        std::vector<std::string> split(const std::string& inValue, char inDelimeter)
        {
            return split(inValue, std::string(1, inDelimeter));
        }

        std::string formatSplittedBlock(const std::string& inValue, const std::string& inDelimeter)
        {
            if (inValue.empty())
            {
                return "";
            }

            std::string block = inValue;

            if (startsWith(block, inDelimeter))
            {
                block.erase(0, 1);
            }

            if (endsWith(block, inDelimeter))
            {
                block.pop_back();
            }

            return trim(block);
        }

        std::vector<std::string> split(const std::string& inValue, const std::string& inDelimeter)
        {
            if (inValue.empty())
            {
                return {};
            }

            int start = 0;

            std::vector<std::string> result = {};

            for (int i = 0; i < inValue.size(); i += inDelimeter.size())
            {
                if (!areEquals(inValue.substr(i, inDelimeter.size()), inDelimeter))
                {
                    continue;
                }

                const std::string block = formatSplittedBlock(
                    inValue.substr(start, i - start),
                    inDelimeter
                );

                start = i;

                if (block.empty())
                {
                    continue;
                }

                result.push_back(block);
            }

            const std::string block = formatSplittedBlock(inValue.substr(start), inDelimeter);

            if (!block.empty())
            {
                result.push_back(block);
            }

            return result;
        }

        std::string join(
            const std::vector<std::string>& inValue,
            const std::string& inJoiner,
            std::uint32_t inStart,
            std::uint32_t inEnd
        )
        {
            const std::uint32_t size  = static_cast<std::uint32_t>(inValue.size() - 1);
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
                result.append(inValue.at(i));
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

        bool toBool(const std::string& inValue)
        {
            if (inValue.empty()) {
                return false;
            }

            return areEquals(inValue, "1") || areEquals(inValue, "true");
        }

        bool isNaN(const std::string& inValue)
        {
            if (inValue.empty())
            {
                return true;
            }

            return std::find_if(
                inValue.begin(), 
                inValue.end(),
                [](unsigned char c) { return !std::isdigit(c) && c != '.' && c != ','; }
            ) != inValue.end();
        }
    }
}