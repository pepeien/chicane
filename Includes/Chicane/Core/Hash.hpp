#pragma once

#include <cstdint>

#include "Chicane/Core.hpp"

namespace Chicane
{
    namespace Hash
    {
        typedef typename std::uint64_t Value;

        Value generate(const void* inData, std::size_t inDataSize, const void* inSeed, std::size_t inSeedSize);
    }
}