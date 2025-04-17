#pragma once

#include "Core.hpp"
#include "Core/Math.hpp"
#include "Runtime/Renderer/Type.hpp"

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