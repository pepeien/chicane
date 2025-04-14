#pragma once

#include "Chicane.hpp"
#include "Chicane/Core/Math.hpp"
#include "Chicane/Runtime/Renderer/Type.hpp"

namespace Chicane
{
    namespace Renderer
    {
        struct CreateInfo
        {
        public:
            Type type = Type::Undefined;
        };
    }
}