#pragma once

#include "Chicane/Core/Essential.hpp"
#include "Chicane/Core/String.hpp"

namespace Chicane
{
    namespace Base64
    {
        CHICANE_CORE std::vector<unsigned char> decodeToUnsigned(const String& inEncoded);
        CHICANE_CORE std::vector<char> decodeToSigned(const String& inEncoded);
        CHICANE_CORE String decode(const String& inEncoded);

        CHICANE_CORE std::vector<unsigned char> encodeToUnsigned(const std::vector<unsigned char>& inDecoded);
        CHICANE_CORE std::vector<unsigned char> encodeToUnsigned(const String& inDecoded);
        CHICANE_CORE std::vector<char> encodeToSigned(const std::vector<char>& inDecoded);
        CHICANE_CORE std::vector<char> encodeToSigned(const String& inDecoded);
        CHICANE_CORE String encode(const std::vector<unsigned char>& inDecoded);
        CHICANE_CORE String encode(const std::vector<char>& inDecoded);
        CHICANE_CORE String encode(const String& inDecoded);
    }
}