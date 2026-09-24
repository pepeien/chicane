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
        if (found != m_names.end())
        {
            return &found->second;
        }

        String name = inValue.trim();
        if (name.startsWith("enum class "))
        {
            name = name.substr(11).trim();
        }
        else if (name.startsWith("enum "))
        {
            name = name.substr(5).trim();
        }

        const auto foundName = m_names.find(name);
        if (foundName != m_names.end())
        {
            return &foundName->second;
        }

        auto tail = [](const String& inName) -> String
        {
            const std::size_t split = inName.lastOf(':');
            if (split == String::npos)
            {
                return inName;
            }

            return inName.substr(split + 1);
        };

        const String nameTail  = tail(name);
        const auto   foundTail = m_names.find(nameTail);
        if (foundTail != m_names.end())
        {
            return &foundTail->second;
        }

        for (const auto& [key, info] : m_names)
        {
            if (key.equals(name) || tail(key).equals(nameTail))
            {
                return &info;
            }
        }

        return nullptr;
    }

    void ReflectionEnumRegistry::add(const ReflectionEnumInfo& inValue)
    {
        m_names[inValue.name] = std::move(inValue);
    }
}