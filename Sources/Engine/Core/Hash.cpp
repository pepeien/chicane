#include "Chicane/Core/Hash.hpp"

#define XXH_STATIC_LINKING_ONLY
#define XXH_IMPLEMENTATION
#include <xxhash.h>

namespace Chicane
{
    namespace Hash
    {
        Value generate(const Subject& inData, const Subject& inSeed)
        {
            return XXH3_64bits_withSeed(inData.data, inData.size, XXH3_64bits(inSeed.data, inSeed.size));
        }
    }
}