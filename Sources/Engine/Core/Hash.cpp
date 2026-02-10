#include "Chicane/Core/Hash.hpp"

#define XXH_STATIC_LINKING_ONLY
#define XXH_IMPLEMENTATION
#include <xxhash.h>

namespace Chicane
{
    namespace Hash
    {
        Value generate(const void* inData, std::size_t inDataSize, const void* inSeed, std::size_t inSeedSize)
        {
            return XXH3_64bits_withSeed(inData, inDataSize, XXH3_64bits(inSeed, inSeedSize));
        }
    }
}