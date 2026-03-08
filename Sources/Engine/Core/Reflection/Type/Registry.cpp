#include "Chicane/Core/Reflection/Type/Registry.hpp"

namespace Chicane
{

    ReflectionTypeRegistry& ReflectionTypeRegistry::getInstance()
    {
        static ReflectionTypeRegistry result;

        return result;
    }

    ReflectionTypeRegistry::ReflectionTypeRegistry()
        : m_indices({}),
          m_names({})
    {}

    const ReflectionTypeRegistry::Names& ReflectionTypeRegistry::getAll() const
    {
        return m_names;
    }

    const ReflectionTypeInfo* ReflectionTypeRegistry::find(const std::type_info& inValue)
    {
        return find(std::type_index(inValue));
    }

    const ReflectionTypeInfo* ReflectionTypeRegistry::find(std::type_index inValue) const
    {
        const auto found = m_indices.find(inValue);

        return found != m_indices.end() ? &found->second : nullptr;
    }

    const ReflectionTypeInfo* ReflectionTypeRegistry::find(const String& inValue) const
    {
        const auto found = m_names.find(inValue);

        return found != m_names.end() ? &found->second : nullptr;
    }

    void ReflectionTypeRegistry::add(const ReflectionTypeInfo& inValue)
    {
        if (inValue.typeIndex.has_value())
        {
            m_indices[inValue.typeIndex.value()] = inValue;
        }

        m_names[inValue.name] = inValue;
    }
}