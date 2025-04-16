#pragma once

#include "Chicane/Core/Essentials.hpp"

namespace Chicane
{
    namespace Base64
    {
        CHICANE std::string encode(const std::vector<std::uint8_t> &inData);
        CHICANE std::vector<uint8_t> decode(const std::string &inData);

        CHICANE std::vector<uint8_t> readFile(const std::string &inData);
        CHICANE void writeFile(const std::vector<uint8_t> &inData, const std::string &inOutput);
    }
}