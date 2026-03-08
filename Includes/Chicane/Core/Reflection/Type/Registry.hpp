#pragma once

#include <unordered_map>

#include "Chicane/Core.hpp"
#include "Chicane/Core/Reflection/Type/Info.hpp"
#include "Chicane/Core/String.hpp"

namespace Chicane
{
    class CHICANE_CORE ReflectionTypeRegistry
    {
    public:
        using Indices = std::unordered_map<std::type_index, ReflectionTypeInfo>;
        using Names   = std::unordered_map<String, ReflectionTypeInfo>;

    public:
        static ReflectionTypeRegistry& getInstance();

    private:
        ReflectionTypeRegistry();
        ~ReflectionTypeRegistry() = default;

        ReflectionTypeRegistry(const ReflectionTypeRegistry&)            = delete;
        ReflectionTypeRegistry& operator=(const ReflectionTypeRegistry&) = delete;

    public:
        const Names& getAll() const;

        const ReflectionTypeInfo* find(const std::type_info& inValue);
        const ReflectionTypeInfo* find(std::type_index inValue) const;
        const ReflectionTypeInfo* find(const String& inValue) const;

        void add(const ReflectionTypeInfo& inValue);

    private:
        Indices m_indices;
        Names   m_names;
    };
}