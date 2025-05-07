#pragma once

#include "Core/Essentials.hpp"

namespace Chicane
{
    namespace String
    {
        // Comparable
        CHICANE bool areEquals(const std::string& inA, const std::string& inB);
        CHICANE bool contains(const std::string& inTarget, const std::string& inValue);
        CHICANE bool startsWith(const std::string& inTarget, const std::string& inValue);
        CHICANE bool endsWith(const std::string& inTarget, const std::string& inValue);

        // Print
        template<typename... T>
        CHICANE std::string sprint(const std::string& inTarget, T... inParams)
        {
            const int bufferSize = static_cast<int>(inTarget.size()) * 10;

            char* buffer = new char[bufferSize];

            int size = std::snprintf(
                buffer,
                bufferSize,
                inTarget.c_str(),
                inParams...
            );

            if (size < 0 || size > bufferSize)
            {
                delete[] buffer;

                throw std::runtime_error("Error while trying to sprint " + inTarget);
            }

            std::string result = std::string(buffer);

            delete[] buffer;

            return result;
        }

        // Helper
        CHICANE std::string trim(const std::string& inTarget);
        CHICANE std::string toLower(const std::string& inTarget);
        CHICANE std::string toUpper(const std::string& inTarget);

        CHICANE std::vector<std::string> split(const std::string& inTarget, char inDelimeter);
        CHICANE std::vector<std::string> split(const std::vector<unsigned char>& inTarget, char inDelimiter);

        CHICANE std::string join(
            const std::vector<std::string>& inTarget,
            const std::string& inJoiner,
            std::uint32_t inStart = 0,
            std::uint32_t inEnd = 0
        );

        CHICANE void sort(std::vector<std::string>& outValue);

        CHICANE bool toBool(const std::string& inTarget);
    }
}