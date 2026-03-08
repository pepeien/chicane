#include "Chicane/Core/Reflection/Enum/Info.hpp"

namespace Chicane
{
    ReflectionEnumInfo::ReflectionEnumInfo(const String& inName, const Enumerators& inEnumerators)
        : name(std::move(inName)),
          enumerators(std::move(inEnumerators))
    {}

    ReflectionEnumInfo::ReflectionEnumInfo()
        : name(""),
          enumerators({})
    {}
}