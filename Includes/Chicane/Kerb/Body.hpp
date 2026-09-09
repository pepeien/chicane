#pragma once

#include <cstdint>

#include "Chicane/Kerb.hpp"

namespace Chicane
{
    namespace Kerb
    {
        struct CHICANE_KERB Body
        {
        public:
            static Body invalid() { return {}; }

        public:
            Body() = default;
            explicit Body(std::uint32_t inId)
                : m_id(inId)
            {}

            bool isValid() const { return m_id != Invalid; }

            std::uint32_t value() const { return m_id; }

            friend inline bool operator==(const Body& inLeft, const Body& inRight)
            {
                return inLeft.m_id == inRight.m_id;
            }

            friend inline bool operator!=(const Body& inLeft, const Body& inRight)
            {
                return inLeft.m_id != inRight.m_id;
            }

        private:
            static constexpr std::uint32_t Invalid = 0xffffffffu;

            std::uint32_t                  m_id = Invalid;
        };
    }
}
