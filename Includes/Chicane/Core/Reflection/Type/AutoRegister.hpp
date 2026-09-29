#pragma once

#include "Chicane/Core.hpp"
#include "Chicane/Core/Reflection.hpp"
#include "Chicane/Core/Reflection/Type/Info.hpp"
#include "Chicane/Core/Reflection/Type/Registry.hpp"

namespace Chicane
{
    struct CHICANE_CORE ReflectionTypeAutoRegister
    {
        explicit ReflectionTypeAutoRegister(const ReflectionTypeInfo& inValue)
        {
            ReflectionTypeRegistry::sInstance().add(std::move(inValue));
        }
    };
}