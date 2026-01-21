#pragma once

#include "Chicane/Core/String.hpp"

#include "Chicane/Renderer.hpp"

namespace Chicane
{
    namespace Renderer
    {
        struct CHICANE_RENDERER Draw
        {
        public:
            using Id = int;

        public:
            static inline constexpr const Id UnknownId = -1;

        public:
            Id     id        = UnknownId;
            String reference = "";
        };
    }
}