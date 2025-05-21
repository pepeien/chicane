#pragma once

#include "Chicane/Core/Essential.hpp"

namespace Chicane
{
    namespace Base64
    {
        CHICANE_CORE std::vector<unsigned char> decodeToUnsigned(const std::string& inEncoded);
        CHICANE_CORE std::vector<char> decodeToSigned(const std::string& inEncoded);
        CHICANE_CORE std::string decode(const std::string& inEncoded);

        CHICANE_CORE std::vector<unsigned char> encodeToUnsigned(const std::vector<unsigned char>& inDecoded);
        CHICANE_CORE std::vector<unsigned char> encodeToUnsigned(const std::string& inDecoded);
        CHICANE_CORE std::vector<char> encodeToSigned(const std::vector<char>& inDecoded);
        CHICANE_CORE std::vector<char> encodeToSigned(const std::string& inDecoded);
        CHICANE_CORE std::string encode(const std::vector<unsigned char>& inDecoded);
        CHICANE_CORE std::string encode(const std::vector<char>& inDecoded);
        CHICANE_CORE std::string encode(const std::string& inDecoded);
    }
}