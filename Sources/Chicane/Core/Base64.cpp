#include "Chicane/Core/Base64.hpp"

namespace Chicane
{
    namespace Base64
    {
        static const std::uint8_t BASE64_TABLE[] = {
            'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M',
            'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z',
            'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm',
            'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z',
            '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '+', '/'
        };

        static unsigned int pos_of_char(const unsigned char chr) {
            if      (chr >= 'A' && chr <= 'Z') return chr - 'A';
            else if (chr >= 'a' && chr <= 'z') return chr - 'a' + ('Z' - 'A')               + 1;
            else if (chr >= '0' && chr <= '9') return chr - '0' + ('Z' - 'A') + ('z' - 'a') + 2;
            else if (chr == '+' || chr == '-') return 62; // Be liberal with input and accept both url ('-') and non-url ('+') base 64 characters (
            else if (chr == '/' || chr == '_') return 63; // Ditto for '/' and '_'
            else throw std::runtime_error("Input is not valid base64-encoded data.");
        }

        std::string encode(const std::vector<std::uint8_t> &inData) {
            std::string __res;
            std::size_t padding;
        
            // Loop takes 3 characters at a time from
            // input_str and stores it in val
            for (std::size_t i = 0; i < inData.size(); i += 3) {
                std::size_t val = 0, count = 0, no_of_bits = 0;
        
                for (std::size_t j = i; j < inData.size() && j <= i + 2; j++) {
                    // binary data of input_str is stored in val
                    val = val << 8;
        
                    // (A + 0 = A) stores character in val
                    val = val | inData[j];
        
                    // calculates how many time loop
                    // ran if "MEN" -> 3 otherwise "ON" -> 2
                    count++;
                }
        
                no_of_bits = count * 8;
        
                // calculates how many "=" to append after res_str.
                padding = no_of_bits % 3;
        
                // extracts all bits from val (6 at a time)
                // and find the value of each block
                while (no_of_bits != 0) {
                    std::size_t temp, index;
                    // retrieve the value of each block
                    if (no_of_bits >= 6) {
                        temp = no_of_bits - 6;
        
                        // binary of 63 is (111111) f
                        index = (val >> temp) & 63;
                        no_of_bits -= 6;
                    } else {
                        temp = 6 - no_of_bits;
        
                        // append zeros to right if bits are less than 6
                        index = (val << temp) & 63;
                        no_of_bits = 0;
                    }
        
                    __res.push_back(BASE64_TABLE[index]);
                }
            }

            for (std::size_t i = 1; i <= padding; i++) {
                __res.push_back('=');
            }
        
            return __res;
        }

        std::vector<uint8_t> decode(const std::string &inData) {
            if (inData.empty()) return {};
        
            std::size_t length_of_string = inData.length();
            std::size_t pos = 0;

            std::size_t approx_length_of_decoded_string = length_of_string / 4 * 3;
            std::string ret;
            ret.reserve(approx_length_of_decoded_string);
        
            while (pos < length_of_string) {
        
            std::size_t pos_of_char_1 = pos_of_char(inData.at(pos+1) );
        
            //
            // Emit the first output byte that is produced in each chunk:
            //
            ret.push_back(static_cast<std::string::value_type>( ( (pos_of_char(inData.at(pos+0)) ) << 2 ) + ( (pos_of_char_1 & 0x30 ) >> 4)));
        
                if ( ( pos + 2 < length_of_string  )      &&  // Check for data that is not padded with equal signs (which is allowed by RFC 2045)
                          inData.at(pos+2) != '=' &&
                          inData.at(pos+2) != '.'     // accept URL-safe base 64 strings, too, so check for '.' also.
                    )
                {
                   //
                   // Emit a chunk's second byte (which might not be produced in the last chunk).
                   //
                      unsigned int pos_of_char_2 = pos_of_char(inData.at(pos+2) );
                      ret.push_back(static_cast<std::string::value_type>( (( pos_of_char_1 & 0x0f) << 4) + (( pos_of_char_2 & 0x3c) >> 2)));
        
                    if ( ( pos + 3 < length_of_string )     &&
                             inData.at(pos+3) != '='  &&
                             inData.at(pos+3) != '.'
                        )
                    {
                        //
                        // Emit a chunk's third byte (which might not be produced in the last chunk).
                        //
                        ret.push_back(static_cast<std::string::value_type>( ( (pos_of_char_2 & 0x03 ) << 6 ) + pos_of_char(inData.at(pos+3))   ));
                    }
                }

                pos += 4;
            }

            return std::vector<uint8_t>(ret.begin(), ret.end());
        }

        std::vector<uint8_t> readFile(const std::string &inPath) {
            std::vector<uint8_t> __arr;
            std::ifstream __file;
            __file.open(inPath, std::ios::binary);

            if (!__file.is_open()) {
                std::cerr << "File: " << inPath << " did not found!" << std::endl;
                exit(EXIT_FAILURE);
            }

            char temp;
            while (__file.read(&temp, sizeof(char))) {
                __arr.push_back(temp);
            }

            __file.close();

            return __arr;
        }

        void writeFile(const std::vector<uint8_t> &inData, const std::string &inOuput) {
            std::ofstream __file;
            __file.open(inOuput, std::ios::binary);

            for (const auto &el : inData) __file.write((char *)&el, sizeof(std::uint8_t));

            __file.close();
        }
    }
}