#pragma once

#include <cstddef>
#include <functional>
#include <optional>
#include <vector>

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
        using TypeIdex = std::optional<std::type_index>;
        using Fields   = std::vector<ReflectionFieldInfo>;
        using Methods  = std::vector<ReflectionMethodInfo>;

    public:
        ReflectionTypeInfo(
            const String&  inName,
            std::size_t    inSize,
            TypeIdex       inTypeIndex,
            const Fields&  inFields,
            const Methods& inMethods
        );
        ReflectionTypeInfo();

    public:
        const ReflectionFieldInfo* findField(const String& inName) const;

        const ReflectionMethodInfo* findMethod(const String& inName) const;

        ReflectionFieldAccessor resolve(const String& inAccessor) const;

    public:
        String      name;
        std::size_t size;
        TypeIdex    typeIndex;
        Fields      fields;
        Methods     methods;
    };
}