#pragma once

#include "Chicane/Core/Essential.hpp"

namespace Chicane
{
    namespace String
    {
        // Comparable
        CHICANE_CORE bool areEquals(const std::string& inA, const std::string& inB);
        CHICANE_CORE bool areEquals(const std::string& inA, char inB);
        CHICANE_CORE bool contains(const std::string& inTarget, const std::string& inValue);
        CHICANE_CORE bool contains(const std::string& inTarget, char inValue);
        CHICANE_CORE bool startsWith(const std::string& inTarget, const std::string& inValue);
        CHICANE_CORE bool startsWith(const std::string& inTarget, char inValue);
        CHICANE_CORE bool endsWith(const std::string& inTarget, const std::string& inValue);

        // Print
        template<typename... T>
        std::string sprint(const std::string& inValue, T... inParams)
        {
            const int bufferSize = static_cast<int>(inValue.size()) * 10;

            char* buffer = new char[bufferSize];

            int size = std::snprintf(
                buffer,
                bufferSize,
                inValue.c_str(),
                inParams...
            );

            if (size < 0 || size > bufferSize)
            {
                delete[] buffer;

                throw std::runtime_error("Error while trying to sprint " + inValue);
            }

            std::string result = std::string(buffer);

            delete[] buffer;

            return result;
        }

        // Helper
        CHICANE_CORE std::string trim(const std::string& inValue);
        CHICANE_CORE std::string toLower(const std::string& inValue);
        CHICANE_CORE std::string toUpper(const std::string& inValue);

        CHICANE_CORE std::vector<std::string> split(
            const std::vector<unsigned char>& inValue,
            const std::string& inDelimiter
        );
        CHICANE_CORE std::vector<std::string> split(
            const std::string& inValue,
            char inDelimeter
        );
        CHICANE_CORE std::vector<std::string> split(
            const std::string& inValue,
            const std::string& inDelimeter
        );

        CHICANE_CORE std::string join(
            const std::vector<std::string>& inValue,
            const std::string& inJoiner,
            std::uint32_t inStart = 0,
            std::uint32_t inEnd = 0
        );

        CHICANE_CORE void sort(std::vector<std::string>& outValue);

        CHICANE_CORE bool toBool(const std::string& inValue);

        CHICANE_CORE bool isNaN(const std::string& inValue);
    }
}