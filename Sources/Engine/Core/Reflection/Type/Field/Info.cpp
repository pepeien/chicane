#include "Chicane/Core/Reflection/Type/Field/Info.hpp"

namespace Chicane
{
    ReflectionFieldInfo::ReflectionFieldInfo(
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
    )
        : property(std::move(inProperty)),
          typeName(std::move(inTypeName)),
          offset(inOffset),
          size(inSize),
          typeIndex(inTypeIndex),
          bIsReflected(bInIsReflected),
          bIsPointer(bInIsPointer),
          bIsIterable(bInIsIterable),
          elementIndex(inElementIndex),
          iterable(std::move(inIterable))
    {}

    ReflectionFieldInfo::ReflectionFieldInfo()
        : property(),
          typeName(""),
          offset(0),
          size(0),
          typeIndex(std::nullopt),
          bIsReflected(false),
          bIsPointer(false),
          bIsIterable(false),
          elementIndex(std::nullopt),
          iterable({})
    {}

    bool ReflectionFieldInfo::containsName(const String& inValue) const
    {
        return property.containsName(inValue);
    }

    const String& ReflectionFieldInfo::getName() const
    {
        return property.getName();
    }

    bool ReflectionFieldInfo::isTransient() const
    {
        return property.isTransient();
    }

    const String& ReflectionFieldInfo::getGroup() const
    {
        return property.group;
    }

    const String& ReflectionFieldInfo::getDescription() const
    {
        return property.description;
    }

    const ReflectionFieldInfo::Names& ReflectionFieldInfo::getNames() const
    {
        return property.names;
    }
}
