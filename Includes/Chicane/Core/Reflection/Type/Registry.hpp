#pragma once

#include <cstddef>
#include <functional>
#include <optional>

#include "Chicane/Core.hpp"
#include "Chicane/Core/Reflection/Type/Info.hpp"
#include "Chicane/Core/String.hpp"

namespace Chicane
{
    class CHICANE_CORE ReflectionTypeRegistry
    {
    public:
        static ReflectionTypeRegistry& get()
        {
            static ReflectionTypeRegistry instance;
            return instance;
        }

        void registerType(ReflectionTypeInfo info)
        {
            if (info.typeIndex.has_value())
            {
                m_byIndex[info.typeIndex.value()] = info;
            }

            m_byName[info.name] = info;
        }

        const ReflectionTypeInfo* find(const std::type_info& ti) const { return find(std::type_index(ti)); }

        const ReflectionTypeInfo* find(std::type_index idx) const
        {
            auto it = m_byIndex.find(idx);
            return it != m_byIndex.end() ? &it->second : nullptr;
        }

        const ReflectionTypeInfo* find(const String& name) const
        {
            auto it = m_byName.find(name);
            return it != m_byName.end() ? &it->second : nullptr;
        }

        const std::unordered_map<String, ReflectionTypeInfo>& all() const { return m_byName; }

    private:
        ReflectionTypeRegistry()  = default;
        ~ReflectionTypeRegistry() = default;

        ReflectionTypeRegistry(const ReflectionTypeRegistry&)            = delete;
        ReflectionTypeRegistry& operator=(const ReflectionTypeRegistry&) = delete;

        std::unordered_map<std::type_index, ReflectionTypeInfo> m_byIndex;
        std::unordered_map<String, ReflectionTypeInfo>          m_byName;
    };
}