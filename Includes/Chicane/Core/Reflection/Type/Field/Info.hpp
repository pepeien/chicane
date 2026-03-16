#pragma once

#include <cstddef>
#include <optional>
#include <typeindex>
#include <vector>

#include "Chicane/Core.hpp"
#include "Chicane/Core/String.hpp"

namespace Chicane
{
    struct CHICANE_CORE ReflectionFieldInfo
    {
    public:
        using TypeIndex = std::optional<std::type_index>;
        using Names     = std::vector<String>;

    public:
        ReflectionFieldInfo(
            Names       inNames,
            String      inTypeName,
            std::size_t inOffset,
            std::size_t inSize,
            TypeIndex   inTypeIndex,
            bool        bInIsReflected,
            bool        bInIsPointer
        );
        ReflectionFieldInfo();

    public:
        bool containsName(const String& inValue) const;

    public:
        Names       names;
        String      typeName;
        std::size_t offset;
        std::size_t size;
        TypeIndex   typeIndex;
        bool        bIsReflected;
        bool        bIsPointer;
    };
}