#include "Chicane/Core/Base64.hpp"

#include "base64.hpp"

namespace Chicane
{
    namespace Base64
    {
        std::vector<unsigned char> decodeToUnsigned(const std::string& inEncoded)
        {
            return base64::decode_into<std::vector<unsigned char>>(inEncoded);
        }

        std::vector<char> decodeToSigned(const std::string& inEncoded)
        {
            return base64::decode_into<std::vector<char>>(inEncoded);
        }

        std::string decode(const std::string& inEncoded)
        {
            return base64::decode_into<std::string>(inEncoded);
        }

        std::vector<unsigned char> encodeToUnsigned(const std::vector<unsigned char>& inDecoded)
        {
            return base64::decode_into<std::vector<unsigned char>>(inDecoded.begin(), inDecoded.end());
        }

        std::vector<unsigned char> encodeToUnsigned(const std::string& inDecoded)
        {
            return base64::decode_into<std::vector<unsigned char>>(inDecoded);
        }

        std::vector<char> encodeToSigned(const std::vector<char>& inDecoded)
        {
            return base64::decode_into<std::vector<char>>(inDecoded.begin(), inDecoded.end());
        }

        std::vector<char> encodeToSigned(const std::string& inDecoded)
        {
            return base64::decode_into<std::vector<char>>(inDecoded);
        }

        std::string encode(const std::vector<unsigned char>& inDecoded)
        {
            return base64::decode_into<std::string>(inDecoded.begin(), inDecoded.end());
        }

        std::string encode(const std::vector<char>& inDecoded)
        {
            return base64::decode_into<std::string>(inDecoded.begin(), inDecoded.end());
        }

        std::string encode(const std::string& inDecoded)
        {
            return base64::decode_into<std::string>(inDecoded);
        }
    }
}