#pragma once

#include <cstddef>
#include <optional>
#include <typeindex>
#include <vector>

#include "Chicane/Core.hpp"
#include "Chicane/Core/Reflection/Property.hpp"
#include "Chicane/Core/Reflection/Type/Field/Iterable.hpp"
#include "Chicane/Core/String.hpp"

namespace Chicane
{
    struct CHICANE_CORE ReflectionFieldInfo
    {
    public:
        using TypeIndex = std::optional<std::type_index>;
        using Names     = ReflectionProperty::Names;

    public:
        ReflectionFieldInfo(
            ReflectionProperty      inProperty,
            String                  inTypeName,
            std::size_t             inOffset,
            std::size_t             inSize,
            TypeIndex               inTypeIndex,
            bool                    bInIsReflected,
            bool                    bInIsPointer,
            bool                    bInIsIterable,
            TypeIndex               inElementIndex,
            ReflectionFieldIterable inIterable
        );
        ReflectionFieldInfo();

    public:
        bool containsName(const String& inValue) const;
        const String& getName() const;
        bool isTransient() const;
        const String& getGroup() const;
        const String& getDescription() const;
        const Names& getNames() const;

    public:
        ReflectionProperty       property;
        String                   typeName;
        std::size_t              offset;
        std::size_t              size;
        TypeIndex                typeIndex;
        bool                     bIsReflected;
        bool                     bIsPointer;
        bool                     bIsIterable;
        TypeIndex                elementIndex;
        ReflectionFieldIterable  iterable;
    };
}
