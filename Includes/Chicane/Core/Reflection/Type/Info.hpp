#pragma once

#include <cstddef>
#include <functional>
#include <optional>

#include "Chicane/Core.hpp"
#include "Chicane/Core/Reflection/Field/Acessor.hpp"
#include "Chicane/Core/Reflection/Field/Info.hpp"
#include "Chicane/Core/Reflection/Method/Info.hpp"
#include "Chicane/Core/String.hpp"

namespace Chicane
{
    struct CHICANE_CORE ReflectionTypeInfo
    {
    public:
        const ReflectionFieldInfo* findField(const String& fieldName) const;

        const ReflectionMethodInfo* findMethod(const String& methodName) const;

        ReflectionFieldAccessor resolve(const String& path) const;

    public:
        String                            name      = "";
        std::size_t                       size      = 0;
        std::optional<std::type_index>    typeIndex = std::nullopt;
        std::vector<ReflectionFieldInfo>  fields    = {};
        std::vector<ReflectionMethodInfo> methods   = {};
    };
}