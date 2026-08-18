#pragma once

#include <any>
#include <cstddef>
#include <functional>
#include <optional>
#include <typeindex>
#include <vector>

#include "Chicane/Core.hpp"
#include "Chicane/Core/Reflection/Type/Field/Acessor.hpp"
#include "Chicane/Core/Reflection/Type/Field/Iterable.hpp"
#include "Chicane/Core/String.hpp"

namespace Chicane
{
    struct CHICANE_CORE ReflectionTypeMethodInfo
    {
    public:
        using TypeIndex          = std::optional<std::type_index>;
        using Params             = std::vector<std::any>;
        using Invoker            = std::function<std::any(void*, Params)>;
        using ContainerResolver  = std::function<const void*(const std::any&)>;

    public:
        ReflectionTypeMethodInfo(
            String                  inName,
            String                  inReturnType,
            std::vector<String>     inParamTypes,
            Invoker                 inInvoker,
            bool                    bInIsIterable,
            TypeIndex               inReturnTypeIndex,
            TypeIndex               inElementIndex,
            std::size_t             inReturnSize,
            ReflectionFieldIterable inIterable,
            ContainerResolver       inContainerResolver
        );
        ReflectionTypeMethodInfo();

    public:
        std::any invoke(void* inInstance, Params inParams = {}) const;
        String toString(const std::any& inValue) const;

        bool isIterable() const;
        ReflectionFieldAccessor makeAccessor(const std::any& inValue) const;

    public:
        String                  name;
        String                  returnType;
        std::vector<String>     paramTypes;
        bool                    bIsIterable;
        TypeIndex               returnTypeIndex;
        TypeIndex               elementIndex;
        std::size_t             returnSize;
        ReflectionFieldIterable iterable;
        ContainerResolver       containerResolver;

    private:
        Invoker m_invoker;
    };
}
