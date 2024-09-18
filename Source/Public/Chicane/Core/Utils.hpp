#pragma once

#include <stdexcept>
#include <string>
#include <vector>

namespace Chicane
{
    namespace Utils
    {
        bool areEquals(const std::string& inA, const std::string& inB);

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

        std::string trim(const std::string& inTarget);
        std::vector<std::string> split(const std::string& inTarget, char inDelimeter);
    }
}