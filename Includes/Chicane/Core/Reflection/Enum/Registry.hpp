#pragma once

#include <unordered_map>

#include "Chicane/Core.hpp"
#include "Chicane/Core/Reflection/Enum/Info.hpp"
#include "Chicane/Core/String.hpp"

namespace Chicane
{
    class CHICANE_CORE ReflectionEnumRegistry
    {
    public:
        using Names = std::unordered_map<String, ReflectionEnumInfo>;

    public:
        static ReflectionEnumRegistry& getInstance();

    private:
        ReflectionEnumRegistry();
        ~ReflectionEnumRegistry() = default;

        ReflectionEnumRegistry(const ReflectionEnumRegistry&)            = delete;
        ReflectionEnumRegistry& operator=(const ReflectionEnumRegistry&) = delete;

    public:
        const Names& getAll() const;

        const ReflectionEnumInfo* find(const String& inValue) const;

        void add(const ReflectionEnumInfo& inValue);

    private:
        Names m_names;
    };
}