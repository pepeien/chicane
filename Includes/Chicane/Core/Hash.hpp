#pragma once

#include <cstdint>

#include "Chicane/Core.hpp"
#include "Chicane/Core/Hash/Subject.hpp"

namespace Chicane
{
    namespace Hash
    {
        typedef typename std::uint64_t Value;

        Value generate(const Subject& inData, const Subject& inSeed);
    }
}