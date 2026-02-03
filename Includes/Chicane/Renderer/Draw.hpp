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
            static constexpr inline const Id    UnknownId        = -1;
            static constexpr inline const char* UnknownReference = "";

        public:
            Id        id        = UnknownId;
            Reference reference = UnknownReference;
        };
    }
}