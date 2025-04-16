#pragma once

#include "Chicane/Core.hpp"
#include "Chicane/Core/Math.hpp"
#include "Chicane/Runtime/Renderer/Type.hpp"

namespace Chicane
{
    namespace Renderer
    {
        struct CHICANE CreateInfo
        {
        public:
            Type type = Type::Undefined;
        };
    }
}