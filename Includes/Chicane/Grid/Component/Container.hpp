#pragma once

#include "Core.hpp"
#include "Grid/Component/Instance.hpp"

namespace Chicane
{
    namespace Grid
    {
        class CHICANE Container : public Component
        {
        public:
            // Tag
            static constexpr const char* TAG_ID = "Container";
        };
    }
}