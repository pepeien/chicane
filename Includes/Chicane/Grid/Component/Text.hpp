#pragma once

#include "Core.hpp"
#include "Grid/Component/Instance.hpp"

namespace Chicane
{
    namespace Grid
    {
        class CHICANE Text : public Component
        {
        public:
            // Tag
            static constexpr const char* TAG_ID = "Text";
        };
    }
}