#pragma once

#include <stdexcept>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

namespace Chicane
{
    namespace String
    {
        // Comparable
        bool areEquals(const std::string& inA, const std::string& inB);

        // Print
        template<typename... T>
        std::string sprint(const std::string& inTarget, T... inParams)
        {
            const int bufferSize = inTarget.size() * 10;

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
        std::string trim(const std::string& inTarget);

        std::vector<std::string> split(const std::string& inTarget, char inDelimeter);
        std::vector<std::string> split(const std::vector<unsigned char>& inTarget, char inDelimiter);

        void sort(std::vector<std::string>& outValue);
    }
}