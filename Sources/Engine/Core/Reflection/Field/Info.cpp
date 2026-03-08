#include "Chicane/Core/Reflection/Field/Info.hpp"

namespace Chicane
{
    ReflectionFieldInfo::ReflectionFieldInfo(
        String      inName,
        String      inTypeName,
        std::size_t inOffset,
        std::size_t inSize,
        TypeIndex   inTypeIndex,
        bool        bInIsReflected,
        bool        bInIsPointer
    )
        : name(std::move(inName)),
          typeName(std::move(inTypeName)),
          offset(std::move(inOffset)),
          size(std::move(inSize)),
          typeIndex(std::move(inTypeIndex)),
          bIsReflected(std::move(bInIsReflected)),
          bIsPointer(std::move(bInIsPointer))
    {}

    ReflectionFieldInfo::ReflectionFieldInfo()
        : name(""),
          typeName(""),
          offset(0),
          size(0),
          typeIndex(std::nullopt),
          bIsReflected(false),
          bIsPointer(false)
    {}
}