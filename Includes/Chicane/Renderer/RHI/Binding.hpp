#pragma once

#include <cstdint>

#include "Chicane/Renderer.hpp"
#include "Chicane/Renderer/RHI/Binding/Type.hpp"

namespace Chicane
{
    namespace Renderer
    {
        namespace RHI
        {
            struct CHICANE_RENDERER Binding
            {
            public:
                bool          bIsVertex         = true;
                bool          bIsFragment       = true;
                bool          bIsPartiallyBound = false;

                std::uint32_t binding = 0;
                BindingType   type    = BindingType::UniformBuffer;
                std::uint32_t count   = 1;
            };
        }
    }
}
