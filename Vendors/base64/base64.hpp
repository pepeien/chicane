
#ifndef __BASE64_HPP__
#define __BASE64_HPP__

#include <string>
#include <vector>
#include <cstdint>

namespace base64 {

/**
 * @brief Encode into Base64 String.
 * @param __arr array of 8-bit array
 * @return Base64 encoded string
 */
std::string encode(const std::vector<uint8_t> &__arr);

/**
 * @brief Decode Base64 encoded string into 8-bit array.
 * @param __str Base64 encoded string
 * @return 8-bit array
 */
std::vector<uint8_t> decode(const std::string &encoded_string);

/**
 * @brief Read file in binary mode.
 * @param __str Name of file
 * @return 8-bit array.
 */
std::vector<uint8_t> read_file(const std::string &__str);

/**
 * @brief Write 8-bit array to file in binary mode.
 * @param __arr 8-bit array
 * @param __str file name
 * @return Nothing.
 */
void write_file(const std::vector<uint8_t> &__arr, const std::string &__str);
}  // namespace base64

#endif  // __BASE64_HPP__
