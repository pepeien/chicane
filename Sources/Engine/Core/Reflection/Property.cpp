#include "Chicane/Core/Reflection/Property.hpp"

namespace Chicane
{
    ReflectionProperty::ReflectionProperty(
        Names  inNames, String inGroup, String inDescription, bool bInIsTransient
    )
        : names(std::move(inNames)),
          group(std::move(inGroup)),
          description(std::move(inDescription)),
          bIsTransient(bInIsTransient)
    {}

    bool ReflectionProperty::containsName(const String& inValue) const
    {
        return std::find_if(
                   names.begin(),
                   names.end(),
                   [&inValue](const String& inName) { return inName.equals(inValue); }
               ) != names.end();
    }

    const String& ReflectionProperty::getName() const
    {
        return names.empty() ? String::sEmpty() : names.front();
    }

    bool ReflectionProperty::isTransient() const
    {
        return bIsTransient;
    }
}
