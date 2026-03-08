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
        using TypeIndex = std::optional<std::type_index>;

    public:
        ReflectionFieldInfo(
            String      inName,
            String      typeName,
            std::size_t inOffset,
            std::size_t inSize,
            TypeIndex   inTypeIndex,
            bool        bInIsReflected,
            bool        bInIsPointer
        );
        ReflectionFieldInfo();

    public:
        String      name;
        String      typeName;
        std::size_t offset;
        std::size_t size;
        TypeIndex   typeIndex;
        bool        bIsReflected;
        bool        bIsPointer;
    };
}