#include "Chicane/Core/String.hpp"

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

            return String::areEquals(inTarget.substr(0, inValue.size()), inValue);
        }

        bool startsWith(const std::string& inTarget, char inValue)
        {
            if (inTarget.empty())
            {
                return false;
            }

            return String::areEquals(inTarget.substr(0, 1), inValue);
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

        std::string trim(const std::string& inValue)
        {
            std::string result = inValue;
            result.erase(result.find_last_not_of(' ') + 1);
            result.erase(0, result.find_first_not_of(' '));

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

        std::vector<std::string> split(
            const std::string& inValue,
            char inDelimeter
        )
        {
            std::vector<std::string> result = {};

            if (inValue.empty())
            {
                return result;
            }

            result.reserve(inValue.size() / 2);

            std::string item = "";

            std::stringstream stream(inValue);
            while (getline(stream, item, inDelimeter))
            {
                result.push_back(String::trim(item));
            }

            return result;
        }

        std::vector<std::string> split(
            const std::string& inValue,
            const std::string& inDelimeter
        )
        {
            if (inValue.empty())
            {
                return { "" };
            }

            std::vector<std::string> result = {};

            std::size_t position = 0;
            std::size_t previous = 0;

            while ((position = inValue.find(inDelimeter, previous)) != std::string::npos) 
            {
                result.push_back(
                    String::trim(
                        inValue.substr(previous, position - previous)
                    )
                );

                previous = position + inDelimeter.length();
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
            std::istringstream iss(inValue);

            double num;
            char leftover;

            if (!(iss >> num) || (iss >> leftover)) {
                return true;
            }

            return std::isnan(num); 
        }
    }
}