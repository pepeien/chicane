#include "Chicane/Core/Reflection/Type/Field/Info.hpp"

namespace Chicane
{
    ReflectionFieldInfo::ReflectionFieldInfo(
        Names                   inNames,
        String                  inTypeName,
        std::size_t             inOffset,
        std::size_t             inSize,
        TypeIndex               inTypeIndex,
        bool                    bInIsReflected,
        bool                    bInIsPointer,
        bool                    bInIsIterable,
        TypeIndex               inElementIndex,
        ReflectionFieldIterable inIterable,
        String                  inGroup,
        String                  inDescription
    )
        : names(std::move(inNames)),
          typeName(std::move(inTypeName)),
          offset(std::move(inOffset)),
          size(std::move(inSize)),
          typeIndex(std::move(inTypeIndex)),
          bIsReflected(std::move(bInIsReflected)),
          bIsPointer(std::move(bInIsPointer)),
          bIsIterable(std::move(bInIsIterable)),
          elementIndex(std::move(inElementIndex)),
          iterable(std::move(inIterable)),
          group(std::move(inGroup)),
          description(std::move(inDescription))
    {}

    ReflectionFieldInfo::ReflectionFieldInfo()
        : names({}),
          typeName(""),
          offset(0),
          size(0),
          typeIndex(std::nullopt),
          bIsReflected(false),
          bIsPointer(false),
          bIsIterable(false),
          elementIndex(std::nullopt),
          iterable({}),
          group(""),
          description("")
    {}

    bool ReflectionFieldInfo::containsName(const String& inValue) const
    {
        return std::find_if(
                   names.begin(),
                   names.end(),
                   [&inValue](const String& inName) { return inName.equals(inValue); }
               ) != names.end();
    }
}