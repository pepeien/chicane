#pragma once

#include <cstddef>
#include <optional>
#include <typeindex>

#include "Chicane/Core.hpp"
#include "Chicane/Core/String.hpp"

namespace Chicane
{
    struct CHICANE_CORE ReflectionFieldInfo
    {
    public:
        String                         name        = "";
        String                         typeName    = "";
        std::size_t                    offset      = 0;
        std::size_t                    size        = 0;
        std::optional<std::type_index> typeIndex   = std::nullopt;
        bool                           isReflected = false;
    };
}