#pragma once

#include <cstdint>

#include "Chicane/Kerb.hpp"

namespace Chicane
{
    namespace Kerb
    {
        struct CHICANE_KERB CollisionGroup
        {
        public:
            using Id = std::uint32_t;

        public:
            static constexpr Id Invalid = ~Id(0);

        public:
            bool isValid() const;

        public:
            Id group    = Invalid;
            Id subGroup = Invalid;
        };
    }
}
