#pragma once

#include "Chicane/Core.hpp"
#include "Chicane/Core/Reflection.hpp"
#include "Chicane/Core/Reflection/Enum/Info.hpp"
#include "Chicane/Core/Reflection/Enum/Registry.hpp"

namespace Chicane
{
    struct CHICANE_CORE ReflectionEnumAutoRegister
    {
        explicit ReflectionEnumAutoRegister(ReflectionEnumInfo info)
        {
            ReflectionEnumRegistry::getInstance().add(std::move(info));
        }
    };
}