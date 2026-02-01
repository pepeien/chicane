#pragma once

#include <vector>

#include "Chicane/Core/String.hpp"

#include "Chicane/Renderer.hpp"

namespace Chicane
{
    namespace Renderer
    {
        struct CHICANE_RENDERER Draw
        {
        public:
            using Id            = std::int32_t;
            using Reference     = String;
            using ReferenceList = std::vector<Reference>;

        public:
            static inline constexpr const Id    UnknownId        = -1;
            static inline constexpr const char* UnknownReference = "";

        public:
            Id        id        = UnknownId;
            Reference reference = UnknownReference;
        };
    }
}