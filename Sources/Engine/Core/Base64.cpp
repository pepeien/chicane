#include "Chicane/Core/Base64.hpp"

#include "base64.hpp"

namespace Chicane
{
    namespace Base64
    {
        std::vector<unsigned char> decodeToUnsigned(const String& inEncoded)
        {
            return base64::decode_into<std::vector<unsigned char>>(
                inEncoded.toStandard()
            );
        }

        std::vector<char> decodeToSigned(const String& inEncoded)
        {
            return base64::decode_into<std::vector<char>>(
                inEncoded.toStandard()
            );
        }

        String decode(const String& inEncoded)
        {
            return base64::decode_into<std::string>(inEncoded.toStandard());
        }

        std::vector<unsigned char>
        encodeToUnsigned(const std::vector<unsigned char>& inDecoded)
        {
            return base64::decode_into<std::vector<unsigned char>>(
                inDecoded.begin(), inDecoded.end()
            );
        }

        std::vector<unsigned char> encodeToUnsigned(const String& inDecoded)
        {
            return base64::decode_into<std::vector<unsigned char>>(
                inDecoded.toStandard()
            );
        }

        std::vector<char> encodeToSigned(const std::vector<char>& inDecoded)
        {
            return base64::decode_into<std::vector<char>>(
                inDecoded.begin(), inDecoded.end()
            );
        }

        std::vector<char> encodeToSigned(const String& inDecoded)
        {
            return base64::decode_into<std::vector<char>>(
                inDecoded.toStandard()
            );
        }

        String encode(const std::vector<unsigned char>& inDecoded)
        {
            return base64::decode_into<std::string>(
                inDecoded.begin(), inDecoded.end()
            );
        }

        String encode(const std::vector<char>& inDecoded)
        {
            return base64::decode_into<std::string>(
                inDecoded.begin(), inDecoded.end()
            );
        }

        String encode(const String& inDecoded)
        {
            return base64::decode_into<std::string>(inDecoded.toStandard());
        }
    }
}