#pragma once

#include "Chicane/Core/Essential.hpp"

namespace Chicane
{
    namespace Base64
    {
        CHICANE_CORE std::string encode(const std::vector<std::uint8_t> &inData);
        CHICANE_CORE std::vector<uint8_t> decode(const std::string &inData);

        CHICANE_CORE std::vector<uint8_t> readFile(const std::string &inData);
        CHICANE_CORE void writeFile(const std::vector<uint8_t> &inData, const std::string &inOutput);
    }
}