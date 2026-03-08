#include "Chicane/Core/Reflection/Enum/Registry.hpp"

namespace Chicane
{
    ReflectionEnumRegistry& ReflectionEnumRegistry::getInstance()
    {
        static ReflectionEnumRegistry result;

        return result;
    }

    ReflectionEnumRegistry::ReflectionEnumRegistry()
        : m_names({})
    {}

    const ReflectionEnumRegistry::Names& ReflectionEnumRegistry::getAll() const
    {
        return m_names;
    }

    const ReflectionEnumInfo* ReflectionEnumRegistry::find(const String& inValue) const
    {
        const auto found = m_names.find(inValue);

        return found != m_names.end() ? &found->second : nullptr;
    }

    void ReflectionEnumRegistry::add(const ReflectionEnumInfo& inValue)
    {
        m_names[inValue.name] = std::move(inValue);
    }
}